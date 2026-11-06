package com.example.myapplication.ui.detail

import android.os.Bundle
import android.view.View
import android.widget.ImageView
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import coil.load
import com.example.myapplication.R
import com.example.myapplication.data.model.NewsArticle
import com.example.myapplication.ui.adapter.NewsAdapter
import com.example.myapplication.viewmodel.NewsViewModel
import kotlinx.coroutines.launch
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

class NewsDetailActivity : AppCompatActivity() {

    private lateinit var viewModel: NewsViewModel
    private var isFavorite = false
    private var currentArticle: NewsArticle? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val showFavorites = intent.getBooleanExtra("show_favorites", false)
        if (showFavorites) {
            showFavoritesList()
            return
        }

        setContentView(R.layout.activity_news_detail)
        setSupportActionBar(findViewById(R.id.toolbar))
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        supportActionBar?.title = "新闻详情"

        viewModel = ViewModelProvider(this, NewsViewModel.Factory())[NewsViewModel::class.java]

        val article = intent.getParcelableExtra<NewsArticle>("news_article")
        if (article != null) {
            displayArticle(article)
        } else {
            val newsId = intent.getLongExtra("news_id", -1)
            if (newsId != -1L) {
                loadFromViewModel(newsId)
            }
        }
    }

    private fun loadFromViewModel(id: Long) {
        lifecycleScope.launch {
            viewModel.articles.collect { articles ->
                val article = articles.find { it.id == id }
                if (article != null) {
                    displayArticle(article)
                }
            }
        }
    }

    private fun displayArticle(article: NewsArticle) {
        currentArticle = article
        isFavorite = article.isFavorite

        findViewById<TextView>(R.id.tv_title).text = article.title
        findViewById<TextView>(R.id.tv_content).text = article.content
        findViewById<TextView>(R.id.tv_author).text = article.author

        val sdf = SimpleDateFormat("yyyy-MM-dd HH:mm", Locale.getDefault())
        findViewById<TextView>(R.id.tv_time).text =
            "${article.viewCount} 阅读 · ${sdf.format(Date(article.publishTime))}"

        val ivFavorite = findViewById<ImageView>(R.id.iv_favorite)
        updateFavoriteIcon(ivFavorite)
        ivFavorite.setOnClickListener {
            viewModel.toggleFavorite(article.id)
            isFavorite = !isFavorite
            updateFavoriteIcon(ivFavorite)
        }

        val ivHeader = findViewById<ImageView>(R.id.iv_header)
        if (article.imageUrl.isNotEmpty()) {
            ivHeader.visibility = View.VISIBLE
            ivHeader.load(article.imageUrl) {
                crossfade(true)
            }
        } else {
            ivHeader.visibility = View.GONE
        }
    }

    private fun updateFavoriteIcon(iv: ImageView) {
        iv.setImageResource(
            if (isFavorite) R.drawable.ic_favorite_filled
            else R.drawable.ic_favorite_border
        )
    }

    private fun showFavoritesList() {
        setContentView(R.layout.activity_favorites)
        setSupportActionBar(findViewById(R.id.toolbar))

        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        supportActionBar?.title = "我的收藏"

        viewModel = ViewModelProvider(this, NewsViewModel.Factory())[NewsViewModel::class.java]

        val rvFavorites = findViewById<RecyclerView>(R.id.rv_favorites)
        val tvEmpty = findViewById<TextView>(R.id.tv_empty)

        rvFavorites.layoutManager = LinearLayoutManager(this)
        val adapter = NewsAdapter(
            onItemClick = { article ->
                viewModel.incrementViewCount(article.id)
                val intent = android.content.Intent(this, NewsDetailActivity::class.java).apply {
                    putExtra("news_article", article)
                }
                startActivity(intent)
            },
            onFavoriteClick = { article ->
                viewModel.toggleFavorite(article.id)
            }
        )
        rvFavorites.adapter = adapter

        lifecycleScope.launch {
            viewModel.articles.collect { articles ->
                val favorites = articles.filter { it.isFavorite }
                adapter.submitList(favorites)
                tvEmpty.visibility = if (favorites.isEmpty()) View.VISIBLE else View.GONE
            }
        }
    }

    override fun onSupportNavigateUp(): Boolean {
        finish()
        return true
    }
}
