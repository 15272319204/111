using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;
using MvcGrades.Models;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection.Emit;
namespace MvcGrades.Data
{
    public class ApplicationDbContext : IdentityDbContext
    {
        public ApplicationDbContext(DbContextOptions<ApplicationDbContext>
            options)
        : base(options)
        {
        }
        public DbSet<Student> Students { get; set; }
        public DbSet<Course> Courses { get; set; }
        public DbSet<Grade> Grades { get; set; }
        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            // 必须先调用 base，以便 Identity 可以配置自己的实体（包括主键）
            base.OnModelCreating(modelBuilder);
            // 配置关系
            modelBuilder.Entity<Grade>()
            .HasOne(g => g.Student)
            .WithMany(s => s.Grades)
            .HasForeignKey(g => g.StudentId);
            modelBuilder.Entity<Grade>()
            .HasOne(g => g.Course)
            .WithMany(c => c.Grades)
            .HasForeignKey(g => g.CourseId);
            // Course -> IdentityUser (Teacher) 关系（IdentityUser 在IdentityDbContext 中已注册）
            modelBuilder.Entity<Course>()
            .HasOne<IdentityUser>(c => c.Teacher)
             .WithMany() // IdentityUser 不定义 Courses 集合
             .HasForeignKey(c => c.TeacherId)
             .OnDelete(DeleteBehavior.SetNull);
            // Student 与 IdentityUser 设为一对一关系
            modelBuilder.Entity<Student>()
            .HasOne<IdentityUser>(s => s.User)
            .WithOne() // IdentityUser 不定义 Student 导航属性
            .HasForeignKey<Student>(s => s.UserId)
            .HasPrincipalKey<IdentityUser>(u => u.Id)
            .OnDelete(DeleteBehavior.SetNull);
            // 为 UserId 添加唯一索引以强制一对一（UserId 可为空）
            modelBuilder.Entity<Student>()
            .HasIndex(s => s.UserId)
            .IsUnique();
        }
    }
}