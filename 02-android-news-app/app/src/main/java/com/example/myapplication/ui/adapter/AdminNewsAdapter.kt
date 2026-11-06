package com.example.myapplication.ui.adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageButton
import android.widget.TextView
import androidx.recyclerview.widget.DiffUtil
import androidx.recyclerview.widget.ListAdapter
import androidx.recyclerview.widget.RecyclerView
import com.example.myapplication.R
import com.example.myapplication.data.model.NewsArticle
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

class AdminNewsAdapter(
    private val onEditClick: (NewsArticle) -> Unit,
    private val onDeleteClick: (NewsArticle) -> Unit
) : ListAdapter<NewsArticle, AdminNewsAdapter.ViewHolder>(DiffCallback) {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_admin_news, parent, false)
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        holder.bind(getItem(position))
    }

    inner class ViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        private val titleText: TextView = itemView.findViewById(R.id.tv_title)
        private val categoryText: TextView = itemView.findViewById(R.id.tv_category)
        private val timeText: TextView = itemView.findViewById(R.id.tv_time)
        private val viewsText: TextView = itemView.findViewById(R.id.tv_views)
        private val btnEdit: ImageButton = itemView.findViewById(R.id.btn_edit)
        private val btnDelete: ImageButton = itemView.findViewById(R.id.btn_delete)

        fun bind(article: NewsArticle) {
            titleText.text = article.title
            categoryText.text = article.category
            viewsText.text = "阅读: ${article.viewCount}"

            val sdf = SimpleDateFormat("MM-dd HH:mm", Locale.getDefault())
            timeText.text = sdf.format(Date(article.publishTime))

            btnEdit.setOnClickListener { onEditClick(article) }
            btnDelete.setOnClickListener { onDeleteClick(article) }
        }
    }

    object DiffCallback : DiffUtil.ItemCallback<NewsArticle>() {
        override fun areItemsTheSame(oldItem: NewsArticle, newItem: NewsArticle) =
            oldItem.id == newItem.id

        override fun areContentsTheSame(oldItem: NewsArticle, newItem: NewsArticle) =
            oldItem == newItem
    }
}
