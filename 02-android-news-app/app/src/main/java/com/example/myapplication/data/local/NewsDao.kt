package com.example.myapplication.data.local

import androidx.room.Dao
import androidx.room.Delete
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import androidx.room.Update
import kotlinx.coroutines.flow.Flow

@Dao
interface NewsDao {

    @Query("SELECT * FROM news ORDER BY publishTime DESC")
    fun getAllNews(): Flow<List<NewsEntity>>

    @Query("SELECT * FROM news WHERE category = :category ORDER BY publishTime DESC")
    fun getNewsByCategory(category: String): Flow<List<NewsEntity>>

    @Query("SELECT * FROM news WHERE isFavorite = 1 ORDER BY publishTime DESC")
    fun getFavoriteNews(): Flow<List<NewsEntity>>

    @Query("SELECT * FROM news WHERE id = :id")
    fun getNewsById(id: Long): NewsEntity?

    @Query("SELECT * FROM news WHERE title LIKE '%' || :keyword || '%' OR content LIKE '%' || :keyword || '%' ORDER BY publishTime DESC")
    fun searchNews(keyword: String): Flow<List<NewsEntity>>

    @Insert(onConflict = OnConflictStrategy.REPLACE)
    fun insert(news: NewsEntity)

    @Update
    fun update(news: NewsEntity)

    @Delete
    fun delete(news: NewsEntity)

    @Query("DELETE FROM news WHERE id = :id")
    fun deleteById(id: Long)

    @Query("UPDATE news SET viewCount = viewCount + 1 WHERE id = :id")
    fun incrementViewCount(id: Long)

    @Query("UPDATE news SET isFavorite = CASE WHEN isFavorite = 1 THEN 0 ELSE 1 END WHERE id = :id")
    fun toggleFavorite(id: Long)

    @Query("SELECT COUNT(*) FROM news")
    fun getNewsCount(): Int

    @Query("SELECT COUNT(*) FROM news WHERE isFavorite = 1")
    fun getFavoriteCount(): Int
}
