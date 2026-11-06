package com.example.myapplication.ui.adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.DiffUtil
import androidx.recyclerview.widget.ListAdapter
import androidx.recyclerview.widget.RecyclerView
import coil.load
import com.example.myapplication.R
import com.example.myapplication.data.model.NewsArticle
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

class NewsAdapter(
    private val onItemClick: (NewsArticle) -> Unit,
    private val onFavoriteClick: (NewsArticle) -> Unit
) : ListAdapter<NewsArticle, NewsAdapter.ViewHolder>(DiffCallback) {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_news, parent, false)
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        holder.bind(getItem(position))
    }

    inner class ViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        private val titleText: TextView = itemView.findViewById(R.id.tv_title)
        private val summaryText: TextView = itemView.findViewById(R.id.tv_summary)
        private val categoryText: TextView = itemView.findViewById(R.id.tv_category)
        private val timeText: TextView = itemView.findViewById(R.id.tv_time)
        private val authorText: TextView = itemView.findViewById(R.id.tv_author)
        private val viewsText: TextView = itemView.findViewById(R.id.tv_views)
        private val favoriteIcon: ImageView = itemView.findViewById(R.id.iv_favorite)
        private val newsImage: ImageView = itemView.findViewById(R.id.iv_news_image)

        fun bind(article: NewsArticle) {
            titleText.text = article.title

            val summary = if (article.content != article.title) {
                article.content.take(100).replace("\n", " ")
            } else {
                article.sourceUrl.ifEmpty { "来源: ${article.author}" }
            }
            summaryText.text = summary

            categoryText.text = article.category
            authorText.text = article.author
            viewsText.text = "${article.viewCount} 阅读"

            val sdf = SimpleDateFormat("MM-dd HH:mm", Locale.getDefault())
            timeText.text = sdf.format(Date(article.publishTime))

            favoriteIcon.setImageResource(
                if (article.isFavorite) R.drawable.ic_favorite_filled
                else R.drawable.ic_favorite_border
            )

            if (article.imageUrl.isNotEmpty()) {
                newsImage.visibility = View.VISIBLE
                newsImage.load(article.imageUrl) {
                    crossfade(true)
                    placeholder(R.color.purple_200)
                    error(R.color.purple_200)
                }
            } else {
                newsImage.visibility = View.GONE
            }

            itemView.setOnClickListener { onItemClick(article) }
            favoriteIcon.setOnClickListener { onFavoriteClick(article) }
        }
    }

    object DiffCallback : DiffUtil.ItemCallback<NewsArticle>() {
        override fun areItemsTheSame(oldItem: NewsArticle, newItem: NewsArticle) =
            oldItem.id == newItem.id

        override fun areContentsTheSame(oldItem: NewsArticle, newItem: NewsArticle) =
            oldItem == newItem
    }
}
