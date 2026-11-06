using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
namespace MvcGrades.Models
{
    public class Grade
    {
        public int Id { get; set; }
        [Required]
        [ForeignKey(nameof(Student))]
        [Display(Name = "学生")]
        public int StudentId { get; set; }
        [Display(Name = "学生")]
        public Student? Student { get; set; }
        [Required]
        [ForeignKey(nameof(Course))]
        [Display(Name = "课程")]
        public int CourseId { get; set; }
        [Display(Name = "课程")]
        public Course? Course { get; set; }
        [Range(0, 100)]
        [Display(Name = "成绩")]
        public decimal Score { get; set; }
    }
}