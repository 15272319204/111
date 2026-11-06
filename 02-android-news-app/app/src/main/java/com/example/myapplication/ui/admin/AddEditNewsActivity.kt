package com.example.myapplication.ui.admin

import android.os.Bundle
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.lifecycleScope
import com.example.myapplication.R
import com.example.myapplication.data.model.Category
import com.example.myapplication.data.model.NewsArticle
import com.example.myapplication.viewmodel.AdminViewModel
import com.google.android.material.textfield.MaterialAutoCompleteTextView
import com.google.android.material.textfield.TextInputEditText
import kotlinx.coroutines.launch

class AddEditNewsActivity : AppCompatActivity() {

    private lateinit var viewModel: AdminViewModel
    private var isEditMode = false
    private var newsId: Long = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_add_edit_news)
        setSupportActionBar(findViewById(R.id.toolbar))

        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        viewModel = ViewModelProvider(this, AdminViewModel.Factory())[AdminViewModel::class.java]

        initViews()
    }

    private fun initViews() {
        val etTitle = findViewById<TextInputEditText>(R.id.et_title)
        val actCategory = findViewById<MaterialAutoCompleteTextView>(R.id.act_category)
        val etAuthor = findViewById<TextInputEditText>(R.id.et_author)
        val etSummary = findViewById<TextInputEditText>(R.id.et_summary)
        val etContent = findViewById<TextInputEditText>(R.id.et_content)
        val etImageUrl = findViewById<TextInputEditText>(R.id.et_image_url)
        val btnSave = findViewById<Button>(R.id.btn_save)

        val categoryNames = Category.DEFAULTS.map { it.name }
        val adapter = ArrayAdapter(this, android.R.layout.simple_dropdown_item_1line, categoryNames)
        actCategory.setAdapter(adapter)

        isEditMode = intent.getBooleanExtra("edit_mode", false)
        if (isEditMode) {
            supportActionBar?.title = "编辑新闻"
            newsId = intent.getLongExtra("news_id", 0)
            etTitle.setText(intent.getStringExtra("news_title") ?: "")
            etContent.setText(intent.getStringExtra("news_content") ?: "")
            etSummary.setText(intent.getStringExtra("news_summary") ?: "")
            actCategory.setText(intent.getStringExtra("news_category") ?: "综合", false)
            etAuthor.setText(intent.getStringExtra("news_author") ?: "管理员")
            etImageUrl.setText(intent.getStringExtra("news_image_url") ?: "")
            btnSave.text = "更新"
        } else {
            supportActionBar?.title = "添加新闻"
        }

        lifecycleScope.launch {
            viewModel.operationResult.collect { result ->
                if (!result.isNullOrBlank()) {
                    Toast.makeText(this@AddEditNewsActivity, result, Toast.LENGTH_SHORT).show()
                    if (result == "添加成功" || result == "更新成功") {
                        finish()
                    }
                    viewModel.clearOperationResult()
                }
            }
        }

        btnSave.setOnClickListener {
            val title = etTitle.text.toString().trim()
            val category = actCategory.text.toString().trim()
            val author = etAuthor.text.toString().trim()
            val summary = etSummary.text.toString().trim()
            val content = etContent.text.toString().trim()
            val imageUrl = etImageUrl.text.toString().trim()

            if (title.isEmpty()) {
                Toast.makeText(this, "请输入新闻标题", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if (content.isEmpty()) {
                Toast.makeText(this, "请输入新闻正文", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            val article = NewsArticle(
                id = if (isEditMode) newsId else 0,
                title = title,
                content = content,
                summary = summary.ifEmpty { title },
                category = category.ifEmpty { "综合" },
                author = author.ifEmpty { "管理员" },
                imageUrl = imageUrl,
                publishTime = System.currentTimeMillis()
            )

            if (isEditMode) {
                viewModel.updateNews(article)
            } else {
                viewModel.addNews(article)
            }
        }
    }

    override fun onSupportNavigateUp(): Boolean {
        finish()
        return true
    }
}
