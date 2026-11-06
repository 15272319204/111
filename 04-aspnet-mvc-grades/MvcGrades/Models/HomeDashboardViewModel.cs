using System.Collections.Generic;
namespace MvcGrades.Models
{
    public class HomeDashboardViewModel
    {
        public int TotalStudents { get; set; }
        public int TotalCourses { get; set; }
        public int TotalGrades { get; set; }
        public decimal? AverageScore { get; set; }
        public IEnumerable<Student> LatestStudents { get; set; } = new
List<Student>();
        public IEnumerable<Course> LatestCourses { get; set; } = new
        List<Course>();
        public IEnumerable<Grade> LatestGrades { get; set; } = new List<Grade>
        ();
    }
}