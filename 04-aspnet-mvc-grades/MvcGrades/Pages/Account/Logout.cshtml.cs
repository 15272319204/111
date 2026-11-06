using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
namespace MvcGrades.Pages.Account
{
    public class LogoutModel : PageModel
    {
        private readonly SignInManager<IdentityUser> _signInManager;
        public LogoutModel(SignInManager<IdentityUser> signInManager)
        {
            _signInManager = signInManager;
        }
        public IActionResult OnGet()
        {
            // 不允许通过 GET 直接登出，保持安全性（可根据需要改为显示确认）
            return RedirectToPage("/Index");
        }
        public async Task<IActionResult> OnPostAsync()
        {
            // 退出登录并重定向到首页
            await _signInManager.SignOutAsync();
            return RedirectToPage("/Index");
        }
    }
}