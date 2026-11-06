using System.ComponentModel.DataAnnotations;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using MvcGrades.Data;
using MvcGrades.Models;
using Microsoft.EntityFrameworkCore;
namespace MvcGrades.Pages.Account
{
    [AllowAnonymous]
    public class RegisterModel : PageModel
    {
        private readonly UserManager<IdentityUser> _userManager;
        private readonly SignInManager<IdentityUser> _signInManager;
        private readonly ILogger<RegisterModel> _logger;
        private readonly RoleManager<IdentityRole> _roleManager;
        private readonly ApplicationDbContext _context;
        public RegisterModel(
        UserManager<IdentityUser> userManager,
        SignInManager<IdentityUser> signInManager,
        RoleManager<IdentityRole> roleManager,
        ILogger<RegisterModel> logger,
        ApplicationDbContext context)
        {
            _userManager = userManager;
            _signInManager = signInManager;
            _roleManager = roleManager;
            _logger = logger;
            _context = context;
        }
        public string? ReturnUrl { get; set; }
        [Required]
        [EmailAddress]
        [Display(Name = "邮箱")]
        [BindProperty]
        public string Email { get; set; } = "";
        [Required]
        [StringLength(100, ErrorMessage = "{0} 必须至少包含 {2} 个字符。",
        MinimumLength = 6)]
        [DataType(DataType.Password)]
        [Display(Name = "密码")]
        [BindProperty]
        public string Password { get; set; } = "";
        [DataType(DataType.Password)]
        [Display(Name = "确认密码")]
        [Compare("Password", ErrorMessage = "密码和确认密码不匹配。")]
        [BindProperty]
        public string ConfirmPassword { get; set; } = "";
        [Display(Name = "姓名（可选）")]
        [BindProperty]
        public string? FullName { get; set; }
        [Display(Name = "学号（可选）")]
        [BindProperty]
        public string? StudentNumber { get; set; }
        public void OnGet(string? returnUrl = null)
        {
            ReturnUrl = returnUrl ?? Url.Content("~/");
        }
        public async Task<IActionResult> OnPostAsync(string? returnUrl = null)
        {
            returnUrl ??= Url.Content("~/");
            if (!ModelState.IsValid) return Page();
            var user = new IdentityUser
            {
                UserName = Email,
                Email = Email,
                EmailConfirmed = true
            };
            var result = await _userManager.CreateAsync(user, Password);
            if (result.Succeeded)
            {
                _logger.LogInformation("用户已创建。");
                // 尝试把新用户加入 Student 角色（如果存在）
                if (await _roleManager.RoleExistsAsync("Student"))
                {
                    await _userManager.AddToRoleAsync(user, "Student");
                }
                // 如果填写了学号，则在 Student 表创建一条关联记录（将 Identity 用户与Student 关联）
if (!string.IsNullOrWhiteSpace(StudentNumber))
                {
                    // 检查是否已有同学号或已有与用户关联的 Student（确保一对一）
                    var existing = await _context.Students
                    .FirstOrDefaultAsync(s => s.StudentNumber ==
                    StudentNumber || s.UserId == user.Id);
                    if (existing != null)
                    {
                        // 如果已有记录但未关联用户，则关联；否则更新姓名等信息
                        existing.UserId = user.Id;
                        existing.Name = string.IsNullOrWhiteSpace(FullName) ?
                        Email : FullName;
                        _context.Students.Update(existing);
                    }
                    else
                    {
                        var student = new Student
                        {
                            Name = string.IsNullOrWhiteSpace(FullName) ? Email :
                        FullName,
                            StudentNumber = StudentNumber,
                            UserId = user.Id
                        };
                        _context.Students.Add(student);
                    }
                    await _context.SaveChangesAsync();
                }
                // 登录
                await _signInManager.SignInAsync(user, isPersistent: false);
                return LocalRedirect(returnUrl);
            }
            foreach (var error in result.Errors)
            {
                ModelState.AddModelError(string.Empty, error.Description);
            }
            return Page();
        }
    }
}