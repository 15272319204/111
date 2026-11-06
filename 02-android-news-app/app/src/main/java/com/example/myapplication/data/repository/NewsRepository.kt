package com.example.myapplication.data.repository

import com.example.myapplication.data.local.AppDatabase
import com.example.myapplication.data.local.NewsEntity
import com.example.myapplication.data.model.Category
import com.example.myapplication.data.model.NewsArticle
import com.example.myapplication.data.remote.ChannelItem
import com.example.myapplication.data.remote.NetworkResult
import com.example.myapplication.data.remote.NewsApiService
import com.example.myapplication.data.remote.NewsItem
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.catch
import kotlinx.coroutines.flow.flow
import kotlinx.coroutines.flow.map
import kotlinx.coroutines.withContext
import java.text.SimpleDateFormat
import java.util.Locale

class NewsRepository(
    private val database: AppDatabase,
    private val apiService: NewsApiService
) {
    private val newsDao = database.newsDao()
    private val dateFormat = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault())

    fun getAllNews(): Flow<List<NewsArticle>> {
        return newsDao.getAllNews().map { entities ->
            entities.map { it.toArticle() }
        }
    }

    fun getNewsByCategory(category: String): Flow<List<NewsArticle>> {
        return newsDao.getNewsByCategory(category).map { entities ->
            entities.map { it.toArticle() }
        }
    }

    fun getFavoriteNews(): Flow<List<NewsArticle>> {
        return newsDao.getFavoriteNews().map { entities ->
            entities.map { it.toArticle() }
        }
    }

    suspend fun getNewsById(id: Long): NewsArticle? = withContext(Dispatchers.IO) {
        newsDao.getNewsById(id)?.toArticle()
    }

    suspend fun insertNews(article: NewsArticle) = withContext(Dispatchers.IO) {
        newsDao.insert(article.toEntity())
    }

    suspend fun updateNews(article: NewsArticle) = withContext(Dispatchers.IO) {
        newsDao.update(article.toEntity())
    }

    suspend fun deleteNews(id: Long) = withContext(Dispatchers.IO) {
        newsDao.deleteById(id)
    }

    suspend fun incrementViewCount(id: Long) = withContext(Dispatchers.IO) {
        newsDao.incrementViewCount(id)
    }

    suspend fun toggleFavorite(id: Long) = withContext(Dispatchers.IO) {
        newsDao.toggleFavorite(id)
    }

    suspend fun getNewsCount(): Int = withContext(Dispatchers.IO) {
        newsDao.getNewsCount()
    }

    suspend fun getFavoriteCount(): Int = withContext(Dispatchers.IO) {
        newsDao.getFavoriteCount()
    }

    fun fetchChannelsFromApi(): Flow<NetworkResult<List<ChannelItem>>> = flow {
        emit(NetworkResult.Loading)
        try {
            val response = apiService.getChannels()
            if (response.isSuccessful) {
                val body = response.body()
                if (body?.showapi_res_code == 0 && body.showapi_res_body?.ret_code == 0) {
                    val channels = body.showapi_res_body.channelList
                    emit(NetworkResult.Success(channels))
                } else {
                    emit(NetworkResult.Error(body?.showapi_res_error ?: "获取频道失败"))
                }
            } else {
                emit(NetworkResult.Error("请求失败: ${response.code()}"))
            }
        } catch (e: Exception) {
            emit(NetworkResult.Error(e.message ?: "网络错误"))
        }
    }.catch { e ->
        emit(NetworkResult.Error(e.message ?: "未知错误"))
    }

    fun fetchNewsFromApi(
        channelId: String? = null,
        channelName: String? = null,
        title: String? = null,
        page: Int = 1
    ): Flow<NetworkResult<List<NewsArticle>>> = flow {
        emit(NetworkResult.Loading)
        try {
            val response = apiService.getNewsList(
                channelId = channelId,
                channelName = channelName,
                title = title,
                page = page,
                maxResult = 30
            )
            if (response.isSuccessful) {
                val body = response.body()
                if (body?.showapi_res_code == 0 && body.showapi_res_body?.ret_code == 0) {
                    val pagebean = body.showapi_res_body.pagebean
                    val newsItems = pagebean?.contentlist ?: emptyList()
                    val articles = newsItems.map { it.toArticle() }
                    withContext(Dispatchers.IO) {
                        for (item in newsItems) {
                            val entity = item.toEntity()
                            val existing = newsDao.getNewsById(entity.id)
                            if (existing == null) {
                                newsDao.insert(entity)
                            }
                        }
                    }
                    emit(NetworkResult.Success(articles))
                } else {
                    emit(NetworkResult.Error(body?.showapi_res_error ?: "获取新闻失败"))
                }
            } else {
                emit(NetworkResult.Error("请求失败: ${response.code()}"))
            }
        } catch (e: Exception) {
            emit(NetworkResult.Error(e.message ?: "网络错误"))
        }
    }.catch { e ->
        emit(NetworkResult.Error(e.message ?: "未知错误"))
    }

    private fun NewsItem.toArticle(): NewsArticle {
        val publishTime = try {
            dateFormat.parse(pubDate)?.time ?: System.currentTimeMillis()
        } catch (e: Exception) {
            System.currentTimeMillis()
        }
        return NewsArticle(
            id = nid.hashCode().toLong(),
            title = title,
            content = desc.ifEmpty { title },
            summary = title,
            category = channelName,
            author = source,
            imageUrl = imageurls.firstOrNull()?.url ?: "",
            sourceUrl = link,
            publishTime = publishTime
        )
    }

    private fun NewsItem.toEntity(): NewsEntity {
        val publishTime = try {
            dateFormat.parse(pubDate)?.time ?: System.currentTimeMillis()
        } catch (e: Exception) {
            System.currentTimeMillis()
        }
        return NewsEntity(
            id = nid.hashCode().toLong(),
            title = title,
            content = desc.ifEmpty { title },
            summary = title,
            category = channelName,
            author = source,
            imageUrl = imageurls.firstOrNull()?.url ?: "",
            sourceUrl = link,
            publishTime = publishTime
        )
    }

    private fun NewsEntity.toArticle() = NewsArticle(
        id = id,
        title = title,
        content = content,
        summary = summary,
        category = category,
        author = author,
        imageUrl = imageUrl,
        sourceUrl = sourceUrl,
        publishTime = publishTime,
        viewCount = viewCount,
        isFavorite = isFavorite
    )

    private fun NewsArticle.toEntity() = NewsEntity(
        id = id,
        title = title,
        content = content,
        summary = summary,
        category = category,
        author = author,
        imageUrl = imageUrl,
        sourceUrl = sourceUrl,
        publishTime = publishTime,
        viewCount = viewCount,
        isFavorite = isFavorite
    )
}
