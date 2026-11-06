using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Rendering;
using Microsoft.EntityFrameworkCore;
using MvcGrades.Data;
using MvcGrades.Models;



namespace MvcGrades.Controllers
{
    [Authorize(Roles = "Admin")]
    public class CoursesController : Controller
    {
        private readonly ApplicationDbContext _context;
        private readonly UserManager<IdentityUser> _userManager;
        public CoursesController(ApplicationDbContext context,
            UserManager<IdentityUser> userManager)
        {
            _context = context;
            _userManager = userManager;
        }

        // GET: Courses
        // 新增学年和学期两个筛选参数 year, term
        public async Task<IActionResult> Index(string? searchString, string? year, string? term, int pageNumber = 1, int pageSize = 5)
        {
            var courses = _context.Courses.Include(c => c.Teacher).AsQueryable();

            if (!string.IsNullOrWhiteSpace(searchString))
            {
                courses = courses.Where(c => c.Name.Contains(searchString));
            }

            // 学年/学期 筛选逻辑
            if (!string.IsNullOrWhiteSpace(year))
            {
                if (string.IsNullOrWhiteSpace(term) || term == "全部学期")
                {
                    // 只按学年过滤（学期全部）
                    courses = courses.Where(c => c.Semester != null && c.Semester.StartsWith(year));
                }
                else
                {
                    // 精确匹配 “大X - 第Y学期”
                    var combined = $"{year} - {term}";
                    courses = courses.Where(c => c.Semester == combined);
                }
            }

            // 固定选项：学年与学期（用于下拉）
            var yearList = new List<SelectListItem>
            {
                new SelectListItem{ Value = "", Text = "全部学年" },
                new SelectListItem{ Value = "大一", Text = "大一", Selected = "大一" == year },
                new SelectListItem{ Value = "大二", Text = "大二", Selected = "大二" == year },
                new SelectListItem{ Value = "大三", Text = "大三", Selected = "大三" == year },
                new SelectListItem{ Value = "大四", Text = "大四", Selected = "大四" == year }
            };
            var termList = new List<SelectListItem>
            {
                new SelectListItem{ Value = "", Text = "全部学期" },
                new SelectListItem{ Value = "第1学期", Text = "第1学期", Selected = "第1学期" == term },
                new SelectListItem{ Value = "第2学期", Text = "第2学期", Selected = "第2学期" == term }
            };

            // 分页
            var totalCount = await courses.CountAsync();
            var totalPages = (int)Math.Ceiling(totalCount / (double)pageSize);
            if (totalPages < 1) totalPages = 1;
            if (pageNumber < 1) pageNumber = 1;
            if (pageNumber > totalPages) pageNumber = totalPages;

            var items = await courses
                .OrderBy(c => c.Id)
                .Skip((pageNumber - 1) * pageSize)
                .Take(pageSize)
                .ToListAsync();

            ViewData["CurrentFilter"] = searchString;
            ViewData["YearList"] = yearList;
            ViewData["TermList"] = termList;
            ViewData["CurrentYear"] = year;
            ViewData["CurrentTerm"] = term;
            ViewData["PageNumber"] = pageNumber;
            ViewData["PageSize"] = pageSize;
            ViewData["TotalPages"] = totalPages;
            ViewData["TotalCount"] = totalCount;

            return View(items);
        }

        // GET: Courses/Details/5
        public async Task<IActionResult> Details(int? id)
        {
            if (id == null) return NotFound();
            var course = await _context.Courses.FirstOrDefaultAsync(m => m.Id == id);
            if (course == null) return NotFound();
            return View(course);
        }

        // GET: Courses/Create
        public async Task<IActionResult> Create()
        {
            await PopulateTeachersDropDownList();
            return View();
        }

        // POST: Courses/Create
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> Create([Bind("Id,Name,Credits,TeacherId,Semester")] Course course)
        {
            if (ModelState.IsValid)
            {
                _context.Add(course);
                await _context.SaveChangesAsync();
                return RedirectToAction(nameof(Index));
            }
            await PopulateTeachersDropDownList(course.TeacherId);
            return View(course);
        }

        // GET: Courses/Edit/5
        public async Task<IActionResult> Edit(int? id)
        {
            if (id == null) return NotFound();
            var course = await _context.Courses.FindAsync(id);
            if (course == null) return NotFound();
            await PopulateTeachersDropDownList(course.TeacherId);
            return View(course);
        }

        // POST: Courses/Edit/5
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> Edit(int id, [Bind("Id,Name,Credits,TeacherId,Semester")] Course course)
        {
            if (id != course.Id) return NotFound();
            if (ModelState.IsValid)
            {
                try
                {
                    _context.Update(course);
                    await _context.SaveChangesAsync();
                }
                catch (DbUpdateConcurrencyException)
                {
                    if (!_context.Courses.Any(e => e.Id == course.Id)) return NotFound();
                    else throw;
                }
                return RedirectToAction(nameof(Index));
            }
            await PopulateTeachersDropDownList(course.TeacherId);
            return View(course);
        }

        private async Task PopulateTeachersDropDownList(object? selectedTeacher = null)
        {
            var teachers = await _userManager.GetUsersInRoleAsync("Teacher");
            var list = teachers.Select(u => new
            {
                u.Id,
                Display = string.IsNullOrEmpty(u.Email) ? u.UserName : u.Email
            }).ToList();
            ViewData["TeacherId"] = new SelectList(list, "Id", "Display", selectedTeacher);
        }

        // GET: Courses/Delete/5
        public async Task<IActionResult> Delete(int? id)
        {
            if (id == null) return NotFound();
            var course = await _context.Courses.FirstOrDefaultAsync(m => m.Id == id);
            if (course == null) return NotFound();
            return View(course);
        }

        // POST: Courses/Delete/5
        [HttpPost, ActionName("Delete")]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> DeleteConfirmed(int id)
        {
            var course = await _context.Courses.FindAsync(id);
            if (course != null) _context.Courses.Remove(course);
            await _context.SaveChangesAsync();
            return RedirectToAction(nameof(Index));
        }

        private bool CourseExists(int id) => _context.Courses.Any(e => e.Id == id);
    }
}