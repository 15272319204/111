using System.ComponentModel.DataAnnotations;
using System.Diagnostics;
using System.ComponentModel.DataAnnotations.Schema;
using Microsoft.AspNetCore.Identity;
namespace MvcGrades.Models
{
    public class Course
    {
        public int Id { get; set; }
        [Required]
        [StringLength(100)]
        [Display(Name = "课程名称")]
        public string Name { get; set; }
        [Display(Name = "学分")]
        public int Credits { get; set; }
        // 存储负责该课程的教师（IdentityUser.Id），可为空
        [Display(Name = "任课教师")]
        public string? TeacherId { get; set; }
        // 学期（可为空，格式可由前端/业务约定）
        [Display(Name = "学期")]
        [StringLength(50)]
        public string? Semester { get; set; }
        // 导航属性到 IdentityUser
        [ForeignKey(nameof(TeacherId))]
        public IdentityUser? Teacher { get; set; }
        public virtual ICollection<Grade>? Grades { get; set; }
    }
}