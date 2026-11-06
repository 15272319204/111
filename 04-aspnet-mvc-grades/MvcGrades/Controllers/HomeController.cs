using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using MvcGrades.Data;
using MvcGrades.Models;
using System.Diagnostics;
namespace MvcGrades.Controllers
{
    [Authorize]
    public class HomeController : Controller
    {
        private readonly ILogger<HomeController> _logger;
        private readonly ApplicationDbContext _context;
        private readonly UserManager<IdentityUser> _userManager;
        public HomeController(ILogger<HomeController> logger,
        ApplicationDbContext context, UserManager<IdentityUser> userManager)
        {
            _logger = logger;
            _context = context;
            _userManager = userManager;
        }
        public async Task<IActionResult> Index()
        {
            var vm = new HomeDashboardViewModel();
            // 获取当前用户 Id（Identity）
            var currentUserId = _userManager.GetUserId(User);
            if (User.IsInRole("Admin"))
            {
                // 管理员：显示全量统计
                vm.TotalStudents = await _context.Students.CountAsync();
                vm.TotalCourses = await _context.Courses.CountAsync();
                vm.TotalGrades = await _context.Grades.CountAsync();
                vm.AverageScore = vm.TotalGrades > 0 ? await
                _context.Grades.AverageAsync(g => g.Score) : (decimal?)null;
                vm.LatestStudents = await _context.Students
                .OrderByDescending(s => s.Id)
                .Take(5)
                .ToListAsync();
                vm.LatestCourses = await _context.Courses
                .OrderByDescending(c => c.Id)
                .Take(5)
                .ToListAsync();
                vm.LatestGrades = await _context.Grades
                .Include(g => g.Student)
                .Include(g => g.Course)
                .OrderByDescending(g => g.Id)
                .Take(5)
                .ToListAsync();
                return View(vm);
            }
            if (User.IsInRole("Teacher"))
            {
                // 教师：仅统计该教师负责的课程与这些课程下的成绩
                var teacherCourses = await _context.Courses
                .Where(c => c.TeacherId == currentUserId)
                .ToListAsync();
                var courseIds = teacherCourses.Select(c => c.Id).ToList();
                vm.TotalStudents = 0; // 不展示学生总数给教师（可按需调整）
                vm.TotalCourses = teacherCourses.Count;
                vm.TotalGrades = await _context.Grades.Where(g =>
                courseIds.Contains(g.CourseId)).CountAsync();
                vm.AverageScore = vm.TotalGrades > 0
                ? await _context.Grades.Where(g =>
                courseIds.Contains(g.CourseId)).AverageAsync(g => g.Score)
                : (decimal?)null;
                vm.LatestStudents = new List<Student>(); // 可按需填充
                vm.LatestCourses = teacherCourses
                .OrderByDescending(c => c.Id)
                .Take(5)
                .ToList();
                vm.LatestGrades = await _context.Grades
                .Include(g => g.Student)
                .Include(g => g.Course)
                .Where(g => courseIds.Contains(g.CourseId))
                .OrderByDescending(g => g.Id)
                .Take(5)
                .ToListAsync();
                return View(vm);
            }
            // Student 或其他角色：仅查看自己的成绩
            var student = await _context.Students.FirstOrDefaultAsync(s =>
            s.UserId == currentUserId);
            if (student != null)
            {
                vm.TotalStudents = 0;
                vm.TotalCourses = 0;
                vm.TotalGrades = await _context.Grades.Where(g => g.StudentId ==
                student.Id).CountAsync();
                vm.AverageScore = vm.TotalGrades > 0
                ? await _context.Grades.Where(g => g.StudentId ==
                student.Id).AverageAsync(g => g.Score)
                : (decimal?)null;
                vm.LatestStudents = new List<Student>();
                vm.LatestCourses = new List<Course>();
                vm.LatestGrades = await _context.Grades
                .Include(g => g.Course)
                .Include(g => g.Student)
                .Where(g => g.StudentId == student.Id)
                .OrderByDescending(g => g.Id)
                .Take(5)
                .ToListAsync();
            }
            return View(vm);
        }
        public IActionResult Privacy()
        {
            return View();
        }
        [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None,
        NoStore = true)]
        public IActionResult Error()
        {
            return View(new ErrorViewModel
            {
                RequestId = Activity.Current?.Id ??
            HttpContext.TraceIdentifier
            });
        }
    }
}
