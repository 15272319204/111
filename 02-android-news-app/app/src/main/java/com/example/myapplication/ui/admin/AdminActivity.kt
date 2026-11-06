package com.example.myapplication.ui.admin

import android.content.Intent
import android.os.Bundle
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.myapplication.R
import com.example.myapplication.data.model.NewsArticle
import com.example.myapplication.ui.adapter.AdminNewsAdapter
import com.example.myapplication.viewmodel.AdminViewModel
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import com.google.android.material.floatingactionbutton.FloatingActionButton
import kotlinx.coroutines.launch

class AdminActivity : AppCompatActivity() {

    private lateinit var viewModel: AdminViewModel
    private lateinit var adapter: AdminNewsAdapter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_admin)
        setSupportActionBar(findViewById(R.id.toolbar))

        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        viewModel = ViewModelProvider(this, AdminViewModel.Factory())[AdminViewModel::class.java]

        initViews()
        observeData()
    }

    private fun initViews() {
        val rvAdminNews = findViewById<RecyclerView>(R.id.rv_admin_news)
        val fabAdd = findViewById<FloatingActionButton>(R.id.fab_add)

        adapter = AdminNewsAdapter(
            onEditClick = { article ->
                val intent = Intent(this, AddEditNewsActivity::class.java).apply {
                    putExtra("edit_mode", true)
                    putExtra("news_id", article.id)
                    putExtra("news_title", article.title)
                    putExtra("news_content", article.content)
                    putExtra("news_summary", article.summary)
                    putExtra("news_category", article.category)
                    putExtra("news_author", article.author)
                    putExtra("news_image_url", article.imageUrl)
                }
                startActivity(intent)
            },
            onDeleteClick = { article ->
                showDeleteConfirmDialog(article)
            }
        )

        rvAdminNews.layoutManager = LinearLayoutManager(this)
        rvAdminNews.adapter = adapter

        fabAdd.setOnClickListener {
            startActivity(Intent(this, AddEditNewsActivity::class.java))
        }
    }

    private fun showDeleteConfirmDialog(article: NewsArticle) {
        MaterialAlertDialogBuilder(this)
            .setTitle("确认删除")
            .setMessage("确定要删除「${article.title}」吗？")
            .setNegativeButton("取消", null)
            .setPositiveButton("删除") { _, _ ->
                viewModel.deleteNews(article.id)
            }
            .show()
    }

    private fun observeData() {
        lifecycleScope.launch {
            viewModel.allArticles.collect { articles ->
                adapter.submitList(articles)
            }
        }

        lifecycleScope.launch {
            viewModel.newsCount.collect { count ->
                findViewById<TextView>(R.id.tv_news_count).text = count.toString()
            }
        }

        lifecycleScope.launch {
            viewModel.favoriteCount.collect { count ->
                findViewById<TextView>(R.id.tv_favorite_count).text = count.toString()
            }
        }

        lifecycleScope.launch {
            viewModel.operationResult.collect { result ->
                if (!result.isNullOrBlank()) {
                    Toast.makeText(this@AdminActivity, result, Toast.LENGTH_SHORT).show()
                    viewModel.clearOperationResult()
                }
            }
        }
    }

    override fun onSupportNavigateUp(): Boolean {
        finish()
        return true
    }
}
