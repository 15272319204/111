using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Rendering;
using Microsoft.EntityFrameworkCore;
using MvcGrades.Data;
using MvcGrades.Models;

namespace MvcGrades.Controllers
{
    [Authorize] // 需要登录；角色内做细化校验
    public class GradesController : Controller
    {
        private readonly ApplicationDbContext _context;
        private readonly UserManager<IdentityUser> _userManager;
        public GradesController(ApplicationDbContext context,
        UserManager<IdentityUser> userManager)
        {
            _context = context;
            _userManager = userManager;
        }

        // GET: Grades
        // 新增 minScore/maxScore 筛选参数
        public async Task<IActionResult> Index(string? searchString, string? sortOrder, int? minScore, int? maxScore, int pageNumber = 1, int pageSize = 5)
        {
            var currentUserId = _userManager.GetUserId(User);
            var query = _context.Grades
                .Include(g => g.Student)
                .Include(g => g.Course)
                .AsQueryable();

            if (User.IsInRole("Admin"))
            {
                // 不做额外限制
            }
            else if (User.IsInRole("Teacher"))
            {
                var courseIds = _context.Courses.Where(c => c.TeacherId == currentUserId).Select(c => c.Id);
                query = query.Where(g => courseIds.Contains(g.CourseId));
            }
            else // student or other
            {
                var student = await _context.Students.FirstOrDefaultAsync(s => s.UserId == currentUserId);
                if (student == null)
                {
                    // 无关联学生，返回空列表（并且分页信息也置为默认）
                    ViewData["CurrentFilter"] = searchString;
                    ViewData["SortOrder"] = sortOrder;
                    ViewData["MinScore"] = minScore;
                    ViewData["MaxScore"] = maxScore;
                    ViewData["PageNumber"] = 1;
                    ViewData["PageSize"] = pageSize;
                    ViewData["TotalPages"] = 1;
                    ViewData["TotalCount"] = 0;
                    ViewData["AverageScore"] = 0.0;
                    ViewData["SelectedAverage"] = 0.0;
                    ViewData["SelectedCount"] = 0;
                    ViewData["SelectedIds"] = Array.Empty<int>();
                    return View(new List<Grade>());
                }
                query = query.Where(g => g.StudentId == student.Id);
            }

            if (!string.IsNullOrWhiteSpace(searchString))
            {
                query = query.Where(g =>
                    (g.Student != null && g.Student.Name.Contains(searchString)) ||
                    (g.Course != null && g.Course.Name.Contains(searchString)) ||
                    g.Score.ToString().Contains(searchString));
            }

            // 应用成绩范围筛选
            if (minScore.HasValue) query = query.Where(g => g.Score >= minScore.Value);
            if (maxScore.HasValue) query = query.Where(g => g.Score <= maxScore.Value);

            // 先计算过滤后的总数与平均分（在分页之前）
            var totalCount = await query.CountAsync();
            var averageScore = await query.Select(g => (double?)g.Score).AverageAsync() ?? 0.0;

            // 应用排序（在分页之前）
            query = sortOrder switch
            {
                "score_desc" => query.OrderByDescending(g => g.Score).ThenBy(g => g.Id),
                "score_asc" => query.OrderBy(g => g.Score).ThenBy(g => g.Id),
                _ => query.OrderBy(g => g.Id)
            };

            // 计算分页
            var totalPages = (int)Math.Ceiling(totalCount / (double)pageSize);
            if (totalPages < 1) totalPages = 1;
            if (pageNumber < 1) pageNumber = 1;
            if (pageNumber > totalPages) pageNumber = totalPages;

            var items = await query
                .Skip((pageNumber - 1) * pageSize)
                .Take(pageSize)
                .ToListAsync();

            ViewData["CurrentFilter"] = searchString;
            ViewData["SortOrder"] = sortOrder;
            ViewData["MinScore"] = minScore;
            ViewData["MaxScore"] = maxScore;
            ViewData["PageNumber"] = pageNumber;
            ViewData["PageSize"] = pageSize;
            ViewData["TotalPages"] = totalPages;
            ViewData["TotalCount"] = totalCount;
            ViewData["AverageScore"] = averageScore;
            ViewData["SelectedAverage"] = 0.0;
            ViewData["SelectedCount"] = 0;
            ViewData["SelectedIds"] = Array.Empty<int>();

            return View(items);
        }

        // POST: 计算选中成绩的平均分（selectedIdsCsv 支持跨页提交）
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> AverageSelected(string? selectedIdsCsv, string? searchString, string? sortOrder, int? minScore, int? maxScore, int pageNumber = 1, int pageSize = 5)
        {
            var currentUserId = _userManager.GetUserId(User);
            var query = _context.Grades
                .Include(g => g.Student)
                .Include(g => g.Course)
                .AsQueryable();

            if (User.IsInRole("Admin"))
            {
                // 不做额外限制
            }
            else if (User.IsInRole("Teacher"))
            {
                var courseIds = _context.Courses.Where(c => c.TeacherId == currentUserId).Select(c => c.Id);
                query = query.Where(g => courseIds.Contains(g.CourseId));
            }
            else // student or other
            {
                var student = await _context.Students.FirstOrDefaultAsync(s => s.UserId == currentUserId);
                if (student == null)
                {
                    ViewData["CurrentFilter"] = searchString;
                    ViewData["SortOrder"] = sortOrder;
                    ViewData["MinScore"] = minScore;
                    ViewData["MaxScore"] = maxScore;
                    ViewData["PageNumber"] = 1;
                    ViewData["PageSize"] = pageSize;
                    ViewData["TotalPages"] = 1;
                    ViewData["TotalCount"] = 0;
                    ViewData["AverageScore"] = 0.0;
                    ViewData["SelectedAverage"] = 0.0;
                    ViewData["SelectedCount"] = 0;
                    ViewData["SelectedIds"] = Array.Empty<int>();
                    return View("Index", new List<Grade>());
                }
                query = query.Where(g => g.StudentId == student.Id);
            }

            if (!string.IsNullOrWhiteSpace(searchString))
            {
                query = query.Where(g =>
                    (g.Student != null && g.Student.Name.Contains(searchString)) ||
                    (g.Course != null && g.Course.Name.Contains(searchString)) ||
                    g.Score.ToString().Contains(searchString));
            }

            // 应用成绩范围筛选（用于显示统计）
            if (minScore.HasValue) query = query.Where(g => g.Score >= minScore.Value);
            if (maxScore.HasValue) query = query.Where(g => g.Score <= maxScore.Value);

            // 计算全量（已过滤）统计，用于分页显示
            var totalCount = await query.CountAsync();
            var averageScore = await query.Select(g => (double?)g.Score).AverageAsync() ?? 0.0;

            // 解析 selectedIdsCsv（CSV 格式），计算选中平均分（只计算当前用户可查看的那些）
            double selectedAverage = 0.0;
            int selectedCount = 0;
            int[] selected = Array.Empty<int>();
            if (!string.IsNullOrWhiteSpace(selectedIdsCsv))
            {
                selected = selectedIdsCsv
                    .Split(new[] { ',', ';' }, StringSplitOptions.RemoveEmptyEntries)
                    .Select(s => { int.TryParse(s, out var v); return v; })
                    .Where(v => v > 0)
                    .Distinct()
                    .ToArray();

                if (selected.Length > 0)
                {
                    var selectedGrades = await _context.Grades
                        .Include(g => g.Course)
                        .Include(g => g.Student)
                        .Where(g => selected.Contains(g.Id))
                        .ToListAsync();

                    // 过滤掉当前用户无权查看的项
                    selectedGrades = selectedGrades.Where(g => CanViewGrade(g)).ToList();

                    selectedCount = selectedGrades.Count;
                    if (selectedCount > 0)
                    {
                        selectedAverage = selectedGrades.Average(g => (double)g.Score);
                    }
                }
            }

            // 重新应用排序到 query（为了返回与 GET 一致的页面）
            query = sortOrder switch
            {
                "score_desc" => query.OrderByDescending(g => g.Score).ThenBy(g => g.Id),
                "score_asc" => query.OrderBy(g => g.Score).ThenBy(g => g.Id),
                _ => query.OrderBy(g => g.Id)
            };

            // 计算分页并返回与 GET 相同页面数据
            var totalPages = (int)Math.Ceiling(totalCount / (double)pageSize);
            if (totalPages < 1) totalPages = 1;
            if (pageNumber < 1) pageNumber = 1;
            if (pageNumber > totalPages) pageNumber = totalPages;

            var items = await query
                .Skip((pageNumber - 1) * pageSize)
                .Take(pageSize)
                .ToListAsync();

            ViewData["CurrentFilter"] = searchString;
            ViewData["SortOrder"] = sortOrder;
            ViewData["MinScore"] = minScore;
            ViewData["MaxScore"] = maxScore;
            ViewData["PageNumber"] = pageNumber;
            ViewData["PageSize"] = pageSize;
            ViewData["TotalPages"] = totalPages;
            ViewData["TotalCount"] = totalCount;
            ViewData["AverageScore"] = averageScore;
            ViewData["SelectedAverage"] = selectedAverage;
            ViewData["SelectedCount"] = selectedCount;
            ViewData["SelectedIds"] = selected;

            return View("Index", items);
        }

        // GET: Grades/Details/5
        public async Task<IActionResult> Details(int? id)
        {
            if (id == null) return NotFound();
            var grade = await _context.Grades
            .Include(g => g.Course)
            .Include(g => g.Student)
            .FirstOrDefaultAsync(m => m.Id == id);
            if (grade == null) return NotFound();
            if (!CanViewGrade(grade)) return Forbid();
            return View(grade);
        }

        // GET: Grades/Create
        [Authorize(Roles = "Teacher,Admin")]
        public IActionResult Create()
        {
            var currentUserId = _userManager.GetUserId(User);
            // 教师仅显示自己所教课程，管理员显示全部
            if (User.IsInRole("Teacher"))
                ViewData["CourseId"] = new SelectList(_context.Courses.Where(c => c.TeacherId == currentUserId), "Id", "Name");
            else
                ViewData["CourseId"] = new SelectList(_context.Courses, "Id", "Name");
            ViewData["StudentId"] = new SelectList(_context.Students, "Id", "Name");
            return View();
        }

        // POST: Grades/Create
        [HttpPost]
        [ValidateAntiForgeryToken]
        [Authorize(Roles = "Teacher,Admin")]
        public async Task<IActionResult>
        Create([Bind("Id,StudentId,CourseId,Score")] Grade grade)
        {
            // 权限再次校验：教师只能为自己课程添加成绩
            if (User.IsInRole("Teacher"))
            {
                var currentUserId = _userManager.GetUserId(User);
                var course = await _context.Courses.FindAsync(grade.CourseId);
                if (course == null || course.TeacherId != currentUserId) return Forbid();
            }
            if (ModelState.IsValid)
            {
                _context.Add(grade);
                await _context.SaveChangesAsync();
                return RedirectToAction(nameof(Index));
            }
            ViewData["CourseId"] = new SelectList(_context.Courses, "Id", "Name", grade.CourseId);
            ViewData["StudentId"] = new SelectList(_context.Students, "Id", "Name", grade.StudentId);
            return View(grade);
        }

        // GET: Grades/Edit/5
        [Authorize(Roles = "Teacher,Admin")]
        public async Task<IActionResult> Edit(int? id)
        {
            if (id == null) return NotFound();
            var grade = await _context.Grades.FindAsync(id);
            if (grade == null) return NotFound();
            // 只能编辑自己课程的成绩（教师）
            if (User.IsInRole("Teacher"))
            {
                var currentUserId = _userManager.GetUserId(User);
                var course = await _context.Courses.FindAsync(grade.CourseId);
                if (course == null || course.TeacherId != currentUserId) return Forbid();
            }
            ViewData["CourseId"] = new SelectList(_context.Courses, "Id", "Name", grade.CourseId);
            ViewData["StudentId"] = new SelectList(_context.Students, "Id", "Name", grade.StudentId);
            return View(grade);
        }

        // POST: Grades/Edit/5
        [HttpPost]
        [ValidateAntiForgeryToken]
        [Authorize(Roles = "Teacher,Admin")]
        public async Task<IActionResult> Edit(int id,
        [Bind("Id,StudentId,CourseId,Score")] Grade grade)
        {
            if (id != grade.Id) return NotFound();
            // 再次校验教师只能编辑自己课程的成绩
            if (User.IsInRole("Teacher"))
            {
                var currentUserId = _userManager.GetUserId(User);
                var course = await _context.Courses.FindAsync(grade.CourseId);
                if (course == null || course.TeacherId != currentUserId) return Forbid();
            }
            if (ModelState.IsValid)
            {
                try
                {
                    _context.Update(grade);
                    await _context.SaveChangesAsync();
                }
                catch (DbUpdateConcurrencyException)
                {
                    if (!_context.Grades.Any(e => e.Id == grade.Id)) return NotFound();
                    else throw;
                }
                return RedirectToAction(nameof(Index));
            }
            ViewData["CourseId"] = new SelectList(_context.Courses, "Id", "Name", grade.CourseId);
            ViewData["StudentId"] = new SelectList(_context.Students, "Id", "Name", grade.StudentId);
            return View(grade);
        }

        // GET: Grades/Delete/5
        [Authorize(Roles = "Teacher,Admin")]
        public async Task<IActionResult> Delete(int? id)
        {
            if (id == null) return NotFound();
            var grade = await _context.Grades
            .Include(g => g.Course)
            .Include(g => g.Student)
            .FirstOrDefaultAsync(m => m.Id == id);
            if (grade == null) return NotFound();
            if (User.IsInRole("Teacher"))
            {
                var currentUserId = _userManager.GetUserId(User);
                if (grade.Course == null || grade.Course.TeacherId != currentUserId) return Forbid();
            }
            return View(grade);
        }

        // POST: Grades/Delete/5
        [HttpPost, ActionName("Delete")]
        [ValidateAntiForgeryToken]
        [Authorize(Roles = "Teacher,Admin")]
        public async Task<IActionResult> DeleteConfirmed(int id)
        {
            var grade = await _context.Grades.FindAsync(id);
            if (grade != null)
            {
                if (User.IsInRole("Teacher"))
                {
                    var currentUserId = _userManager.GetUserId(User);
                    var course = await _context.Courses.FindAsync(grade.CourseId);
                    if (course == null || course.TeacherId != currentUserId) return Forbid();
                }
                _context.Grades.Remove(grade);
                await _context.SaveChangesAsync();
            }
            return RedirectToAction(nameof(Index));
        }

        private bool CanViewGrade(Grade grade)
        {
            var currentUserId = _userManager.GetUserId(User);
            if (User.IsInRole("Admin")) return true;
            if (User.IsInRole("Teacher"))
            {
                return grade.Course != null && grade.Course.TeacherId == currentUserId;
            }
            // student
            return grade.Student != null && grade.Student.UserId == currentUserId;
        }
        private bool GradeExists(int id)
        {
            return _context.Grades.Any(e => e.Id == id);
        }
    }
}
