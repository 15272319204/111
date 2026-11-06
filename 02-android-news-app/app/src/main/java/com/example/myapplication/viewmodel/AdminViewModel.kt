package com.example.myapplication.viewmodel

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import com.example.myapplication.NewsApplication
import com.example.myapplication.data.model.NewsArticle
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch

class AdminViewModel : ViewModel() {

    private val repository = NewsApplication.instance.repository

    private val _allArticles = MutableStateFlow<List<NewsArticle>>(emptyList())
    val allArticles: StateFlow<List<NewsArticle>> = _allArticles.asStateFlow()

    private val _newsCount = MutableStateFlow(0)
    val newsCount: StateFlow<Int> = _newsCount.asStateFlow()

    private val _favoriteCount = MutableStateFlow(0)
    val favoriteCount: StateFlow<Int> = _favoriteCount.asStateFlow()

    private val _operationResult = MutableStateFlow<String?>(null)
    val operationResult: StateFlow<String?> = _operationResult.asStateFlow()

    init {
        loadAllNews()
    }

    fun loadAllNews() {
        viewModelScope.launch {
            repository.getAllNews().collect { articles ->
                _allArticles.value = articles
            }
        }
        viewModelScope.launch {
            _newsCount.value = repository.getNewsCount()
            _favoriteCount.value = repository.getFavoriteCount()
        }
    }

    fun addNews(article: NewsArticle) {
        viewModelScope.launch {
            try {
                repository.insertNews(article)
                _operationResult.value = "添加成功"
                loadAllNews()
            } catch (e: Exception) {
                _operationResult.value = "添加失败: ${e.message}"
            }
        }
    }

    fun updateNews(article: NewsArticle) {
        viewModelScope.launch {
            try {
                repository.updateNews(article)
                _operationResult.value = "更新成功"
                loadAllNews()
            } catch (e: Exception) {
                _operationResult.value = "更新失败: ${e.message}"
            }
        }
    }

    fun deleteNews(id: Long) {
        viewModelScope.launch {
            try {
                repository.deleteNews(id)
                _operationResult.value = "删除成功"
                loadAllNews()
            } catch (e: Exception) {
                _operationResult.value = "删除失败: ${e.message}"
            }
        }
    }

    fun clearOperationResult() {
        _operationResult.value = null
    }

    class Factory : ViewModelProvider.Factory {
        @Suppress("UNCHECKED_CAST")
        override fun <T : ViewModel> create(modelClass: Class<T>): T {
            return AdminViewModel() as T
        }
    }
}
