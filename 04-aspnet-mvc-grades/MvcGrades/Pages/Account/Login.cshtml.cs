using System.ComponentModel.DataAnnotations;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
namespace MvcGrades.Pages.Account
{
    [AllowAnonymous]
    public class LoginModel : PageModel
    {
        private readonly SignInManager<IdentityUser> _signInManager;
        private readonly UserManager<IdentityUser> _userManager;
        private readonly ILogger<LoginModel> _logger;
        [BindProperty(SupportsGet = true)]
        public string? ReturnUrl { get; set; }
        [Required]
        [Display(Name = "邮箱或用户名")]
        [BindProperty]
        public string Login { get; set; } = string.Empty;
        [Required]
        [DataType(DataType.Password)]
        [Display(Name = "密码")]
        [BindProperty]
        public string Password { get; set; } = string.Empty;
        [Display(Name = "记住我")]
        [BindProperty]
        public bool RememberMe { get; set; }
        public LoginModel(
        SignInManager<IdentityUser> signInManager,
        UserManager<IdentityUser> userManager,
        ILogger<LoginModel> logger)
        {
            _signInManager = signInManager;
            _userManager = userManager;
            _logger = logger;
        }
        public void OnGet(string? returnUrl = null)
        {
            ReturnUrl = returnUrl ?? Url.Content("~/");
        }
        public async Task<IActionResult> OnPostAsync(string? returnUrl = null)
        {
            returnUrl ??= ReturnUrl ?? Url.Content("~/");
            if (!ModelState.IsValid)
            {
                return Page();
            }
            var result = await _signInManager.PasswordSignInAsync(Login,
            Password, RememberMe, lockoutOnFailure: false);
            if (result.Succeeded)
            {
                _logger.LogInformation("用户登录成功。");
                if (!string.IsNullOrEmpty(returnUrl) &&
                Url.IsLocalUrl(returnUrl))
                {
                    return LocalRedirect(returnUrl);
                }
                return Redirect("~/");
            }
            if (result.IsLockedOut)
            {
                ModelState.AddModelError(string.Empty, "账户被锁定。");
                return Page();
            }
            ModelState.AddModelError(string.Empty, "登录失败：账号或密码错误。");
            return Page();
        }
    }
}