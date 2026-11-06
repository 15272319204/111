using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Rendering;
using Microsoft.EntityFrameworkCore;
using MvcGrades.Data;
using MvcGrades.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MvcGrades.Controllers
{
    [Authorize(Roles = "Admin")]
    public class StudentsController : Controller
    {
        private readonly ApplicationDbContext _context;
        private readonly UserManager<IdentityUser> _userManager;

        public StudentsController(ApplicationDbContext context,
            UserManager<IdentityUser> userManager)
        {
            _context = context;
            _userManager = userManager;
        }

        // GET: Students
        public async Task<IActionResult> Index(string? searchString, int pageNumber = 1, int pageSize = 5)
        {
            var students = _context.Students
                .Include(s => s.User)
                .AsQueryable();

            if (!string.IsNullOrWhiteSpace(searchString))
            {
                students = students.Where(s =>
                    s.Name.Contains(searchString) ||
                    s.StudentNumber.Contains(searchString) ||
                    (s.User != null && (
                        (s.User.Email != null && s.User.Email.Contains(searchString)) ||
                        (s.User.UserName != null && s.User.UserName.Contains(searchString))
                    )));
            }

            // 分页计算
            var totalCount = await students.CountAsync();
            var totalPages = (int)Math.Ceiling(totalCount / (double)pageSize);
            if (totalPages < 1) totalPages = 1;
            if (pageNumber < 1) pageNumber = 1;
            if (pageNumber > totalPages) pageNumber = totalPages;

            var items = await students
                .OrderBy(s => s.Id)
                .Skip((pageNumber - 1) * pageSize)
                .Take(pageSize)
                .ToListAsync();

            ViewData["CurrentFilter"] = searchString;
            ViewData["PageNumber"] = pageNumber;
            ViewData["PageSize"] = pageSize;
            ViewData["TotalPages"] = totalPages;
            ViewData["TotalCount"] = totalCount;

            return View(items);
        }

        // GET: Students/Details/5
        public async Task<IActionResult> Details(int? id)
        {
            if (id == null) return NotFound();
            var student = await _context.Students.FirstOrDefaultAsync(m => m.Id == id);
            if (student == null) return NotFound();
            return View(student);
        }

        // GET: Students/Create
        public async Task<IActionResult> CreateAsync()
        {
            await PopulateStudentsDropDownList(null);
            return View();
        }

        // POST: Students/Create
        // To protect from overposting attacks, enable the specific propertiesyou want to bind to.
        // For more details, see http://go.microsoft.com/fwlink/?LinkId=317598.
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> Create([Bind("Id,Name,StudentNumber,UserId")] Student student)
        {
            if (ModelState.IsValid)
            {
                _context.Add(student);
                await _context.SaveChangesAsync();
                return RedirectToAction(nameof(Index));
            }
            await PopulateStudentsDropDownList(student.UserId);
            return View(student);
        }

        /// <summary>
        /// 加载“学生”角色且未被 Student 表关联的账号供下拉选择。
        /// 若传入 currentStudentUserId，则允许把当前被编辑学生已关联的账号包含在列表中。
        /// 若没有可用账号，将设置 ViewBag.NoAvailableAccount = true 并提供一个占位选项。
        /// </summary>
        private async Task PopulateStudentsDropDownList(string? selectedUserId, string? currentStudentUserId = null)
        {
            // 获取所有属于 Student 角色的用户
            var studentUsers = await _userManager.GetUsersInRoleAsync("Student");
            // 获取已经在 Students 表中关联的 UserId（非空）
            var usedUserIds = await _context.Students
                .Where(s => !string.IsNullOrEmpty(s.UserId))
                .Select(s => s.UserId!)
                .ToListAsync();

            // 筛选出未被 Student 表占用的账号，或为当前编辑学生已持有的账号（允许保留）
            var available = studentUsers
                .Where(u => !usedUserIds.Contains(u.Id) || u.Id == currentStudentUserId)
                .Select(u => new
                {
                    u.Id,
                    Display = string.IsNullOrEmpty(u.Email) ? u.UserName ?? u.Id : u.Email
                })
                .OrderBy(x => x.Display)
                .ToList();

            if (available.Count == 0)
            {
                // 无可用账号：提供占位项并在视图中提示
                ViewData["UserId"] = new SelectList(new[] { new { Id = "", Display = "（无可用学生账号）" } }, "Id", "Display");
                ViewBag.NoAvailableAccount = true;
            }
            else
            {
                ViewData["UserId"] = new SelectList(available, "Id", "Display", selectedUserId);
                ViewBag.NoAvailableAccount = false;
            }
        }

        // GET: Students/Edit/5
        public async Task<IActionResult> Edit(int? id)
        {
            if (id == null) return NotFound();
            var student = await _context.Students.FindAsync(id);
            if (student == null) return NotFound();
            // 加载下拉：允许包含当前学生已关联的账号
            await PopulateStudentsDropDownList(student.UserId, student.UserId);
            return View(student);
        }

        // POST: Students/Edit/5
        // To protect from overposting attacks, enable the specific properties you want to bind to.
        // For more details, see http://go.microsoft.com/fwlink/?LinkId=317598.
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> Edit(int id, [Bind("Id,Name,StudentNumber,UserId")] Student student)
        {
            if (id != student.Id) return NotFound();

            if (!string.IsNullOrEmpty(student.UserId))
            {
                var conflict = await _context.Students
                    .AsNoTracking()
                    .FirstOrDefaultAsync(s => s.UserId == student.UserId && s.Id != student.Id);
                if (conflict != null)
                {
                    ModelState.AddModelError("UserId", "该账号已被其他学生关联，请选择其它账号或先解除关联。");
                }
            }

            if (ModelState.IsValid)
            {
                try
                {
                    var dbEntity = await _context.Students.FindAsync(id);
                    if (dbEntity == null) return NotFound();
                    dbEntity.Name = student.Name;
                    dbEntity.StudentNumber = student.StudentNumber;
                    dbEntity.UserId = string.IsNullOrEmpty(student.UserId) ? null : student.UserId;
                    _context.Update(dbEntity);
                    await _context.SaveChangesAsync();
                }
                catch (DbUpdateConcurrencyException)
                {
                    if (!StudentExists(student.Id)) return NotFound();
                    else throw;
                }
                return RedirectToAction(nameof(Index));
            }

            // 如果验证失败，重新填充下拉并返回视图
            await PopulateStudentsDropDownList(student.UserId, student.UserId);
            return View(student);
        }

        // GET: Students/Delete/5
        public async Task<IActionResult> Delete(int? id)
        {
            if (id == null) return NotFound();
            var student = await _context.Students.FirstOrDefaultAsync(m => m.Id == id);
            if (student == null) return NotFound();
            return View(student);
        }

        // POST: Students/Delete/5
        [HttpPost, ActionName("Delete")]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> DeleteConfirmed(int id)
        {
            var student = await _context.Students.FindAsync(id);
            if (student != null) _context.Students.Remove(student);
            await _context.SaveChangesAsync();
            return RedirectToAction(nameof(Index));
        }

        private bool StudentExists(int id) => _context.Students.Any(e => e.Id == id);
    }
}