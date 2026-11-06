using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc.Authorization;
using Microsoft.EntityFrameworkCore;
using MvcGrades.Data;
var builder = WebApplication.CreateBuilder(args);
// 添加数据库上下文
builder.Services.AddDbContext<ApplicationDbContext>(options =>
options.UseSqlServer(builder.Configuration.GetConnectionString("DefaultConnection")));
// 添加 Identity 服务
builder.Services.AddDefaultIdentity<IdentityUser>(options =>
options.SignIn.RequireConfirmedAccount = false)
.AddRoles<IdentityRole>()
.AddEntityFrameworkStores<ApplicationDbContext>();
builder.Services.ConfigureApplicationCookie(options =>
{
    options.LoginPath = "/Account/Login"; // 指向你的自定义登录页
    options.AccessDeniedPath = "/Account/Login";
});
// 全局授权过滤器：未登录自动跳转到登录页
builder.Services.AddControllersWithViews(options =>
{
    var policy = new AuthorizationPolicyBuilder()
    .RequireAuthenticatedUser()
    .Build();
    options.Filters.Add(new AuthorizeFilter(policy));
});
builder.Services.AddRazorPages(options =>
{
    options.Conventions.AuthorizeFolder("/");
    // 明确允许匿名访问登录与注册页，防止重定向循环
    options.Conventions.AllowAnonymousToPage("/Account/Login");
    options.Conventions.AllowAnonymousToPage("/Account/Register");
});
// 添加 Razor Pages（必须启用 Identity UI 路由）
builder.Services.AddRazorPages();
// MVC 控制器
builder.Services.AddControllersWithViews();
var app = builder.Build();
// 生产环境异常处理
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Home/Error");
    app.UseHsts();
}
app.UseHttpsRedirection();
app.UseRouting();
app.UseAuthentication();
app.UseAuthorization();
app.MapStaticAssets();
// 映射控制器路由
app.MapControllerRoute(
name: "default",
pattern: "{controller=Home}/{action=Index}/{id?}")
.WithStaticAssets();
// 映射 Razor Pages（Identity UI 使用 Razor Pages 路由）
app.MapRazorPages();
// 角色与初始管理员账号种子（首次运行时创建）
using (var scope = app.Services.CreateScope())
{
    var services = scope.ServiceProvider;
    var roleManager = services.GetRequiredService<RoleManager<IdentityRole>>();
    var userManager = services.GetRequiredService<UserManager<IdentityUser>>();
    string[] roles = new[] { "Student", "Teacher", "Admin" };
    foreach (var role in roles)
    {
        if (!await roleManager.RoleExistsAsync(role))
        {
            await roleManager.CreateAsync(new IdentityRole(role));
        }
    }
    // 创建一个默认管理员（仅在不存在时创建）
    var adminEmail = builder.Configuration["AdminUser:Email"] ?? "wc233@qq.com";
    var adminPassword = builder.Configuration["AdminUser:Password"] ?? "Admin@123";
    var admin = await userManager.FindByEmailAsync(adminEmail);
    if (admin == null)
    {
        admin = new IdentityUser
        {
            UserName = adminEmail,
            Email = adminEmail,
            EmailConfirmed = true
        };
        var result = await userManager.CreateAsync(admin, adminPassword);
        if (result.Succeeded)
        {
            await userManager.AddToRoleAsync(admin, "Admin");
            await userManager.AddToRoleAsync(admin, "Teacher"); // 根据需要赋予角色
        }
    }
}
app.Run();