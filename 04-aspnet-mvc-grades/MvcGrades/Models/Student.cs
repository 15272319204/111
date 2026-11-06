using Microsoft.AspNetCore.Identity;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Diagnostics;
namespace MvcGrades.Models
{
    public class Student
    {
        public int Id { get; set; }
        [Required]
        [StringLength(50)]
        [Display(Name = "姓名")]
        public string Name { get; set; }
        [Required]
        [StringLength(20)]
        [Display(Name = "学号")]
        public string StudentNumber { get; set; }
        // 关联到 AspNetUsers 表的 Identity 用户 Id（可为空）
        public string? UserId { get; set; }
        // 导航属性到 IdentityUser
        [ForeignKey(nameof(UserId))]
        public IdentityUser? User { get; set; }
        // 标记为可空并初始化为空集合，避免在创建时触发必填验证
        public virtual ICollection<Grade>? Grades { get; set; }
    }
}