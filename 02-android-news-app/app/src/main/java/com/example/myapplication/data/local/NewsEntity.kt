package com.example.myapplication.data.local

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity(tableName = "news")
data class NewsEntity(
    @PrimaryKey(autoGenerate = true)
    val id: Long = 0,
    val title: String,
    val content: String,
    val summary: String,
    val category: String = "综合",
    val author: String = "管理员",
    val imageUrl: String = "",
    val sourceUrl: String = "",
    val publishTime: Long = System.currentTimeMillis(),
    val viewCount: Int = 0,
    val isFavorite: Boolean = false
)
