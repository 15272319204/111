using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace MvcGrades.Migrations
{
    /// <inheritdoc />
    public partial class AddCourseSemester : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "Semester",
                table: "Courses",
                type: "nvarchar(50)",
                maxLength: 50,
                nullable: true);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Semester",
                table: "Courses");
        }
    }
}
