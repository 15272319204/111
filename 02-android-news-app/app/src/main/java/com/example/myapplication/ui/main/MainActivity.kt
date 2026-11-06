package com.example.myapplication.ui.main

import android.content.Intent
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.view.Menu
import android.view.MenuItem
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.myapplication.R
import com.example.myapplication.data.model.NewsArticle
import com.example.myapplication.ui.adapter.NewsAdapter
import com.example.myapplication.ui.admin.AdminActivity
import com.example.myapplication.ui.detail.NewsDetailActivity
import com.example.myapplication.viewmodel.NewsViewModel
import com.google.android.material.chip.Chip
import com.google.android.material.chip.ChipGroup
import kotlinx.coroutines.launch

class MainActivity : AppCompatActivity() {

    private lateinit var viewModel: NewsViewModel
    private lateinit var adapter: NewsAdapter
    private lateinit var rvNews: RecyclerView
    private lateinit var chipGroup: ChipGroup

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        setSupportActionBar(findViewById(R.id.toolbar))

        viewModel = ViewModelProvider(this, NewsViewModel.Factory())[NewsViewModel::class.java]

        initViews()
        observeData()
    }

    private fun initViews() {
        rvNews = findViewById(R.id.rv_news)
        chipGroup = findViewById(R.id.chip_group)
        val swipeRefresh = findViewById<androidx.swiperefreshlayout.widget.SwipeRefreshLayout>(R.id.swipe_refresh)
        val etSearch = findViewById<com.google.android.material.textfield.TextInputEditText>(R.id.et_search)

        adapter = NewsAdapter(
            onItemClick = { article ->
                viewModel.incrementViewCount(article.id)
                val intent = Intent(this, NewsDetailActivity::class.java).apply {
                    putExtra("news_article", article)
                }
                startActivity(intent)
            },
            onFavoriteClick = { article ->
                viewModel.toggleFavorite(article.id)
            }
        )

        rvNews.layoutManager = LinearLayoutManager(this)
        rvNews.adapter = adapter

        swipeRefresh.setOnRefreshListener {
            viewModel.refreshFromApi()
        }

        etSearch.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable?) {
                viewModel.searchNews(s?.toString() ?: "")
            }
        })
    }

    private fun observeData() {
        lifecycleScope.launch {
            viewModel.categories.collect { categories ->
                chipGroup.removeAllViews()
                for ((index, category) in categories.withIndex()) {
                    val chip = Chip(this@MainActivity).apply {
                        text = category.displayName
                        isCheckable = true
                        setOnClickListener {
                            val channels = viewModel.apiChannels.value
                            if (index < channels.size) {
                                val channel = channels[index]
                                viewModel.selectChannel(channel.channelId, channel.name)
                            } else {
                                viewModel.selectChannel(null, category.name)
                            }
                        }
                    }
                    chipGroup.addView(chip)
                }
                if (chipGroup.childCount > 0) {
                    (chipGroup.getChildAt(0) as? Chip)?.isChecked = true
                }
            }
        }

        lifecycleScope.launch {
            viewModel.isRefreshing.collect { isRefreshing ->
                findViewById<androidx.swiperefreshlayout.widget.SwipeRefreshLayout>(R.id.swipe_refresh)
                    .isRefreshing = isRefreshing
            }
        }

        lifecycleScope.launch {
            viewModel.articles.collect { articles ->
                adapter.submitList(articles)
            }
        }

        lifecycleScope.launch {
            viewModel.isLoading.collect { isLoading ->
                findViewById<android.widget.ProgressBar>(R.id.progress_bar).visibility =
                    if (isLoading && adapter.itemCount == 0) android.view.View.VISIBLE
                    else android.view.View.GONE
            }
        }

        lifecycleScope.launch {
            viewModel.errorMessage.collect { error ->
                if (!error.isNullOrBlank()) {
                    Toast.makeText(this@MainActivity, error, Toast.LENGTH_SHORT).show()
                    viewModel.clearError()
                }
            }
        }
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.main_menu, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        return when (item.itemId) {
            R.id.action_admin -> {
                startActivity(Intent(this, AdminActivity::class.java))
                true
            }
            R.id.action_favorites -> {
                val intent = Intent(this, NewsDetailActivity::class.java).apply {
                    putExtra("show_favorites", true)
                }
                startActivity(intent)
                true
            }
            else -> super.onOptionsItemSelected(item)
        }
    }
}
