package com.example.myapplication.data.model

data class Category(
    val name: String,
    val displayName: String,
    val icon: Int = 0
) {
    companion object {
        val DEFAULTS = listOf(
            Category("综合", "综合"),
            Category("科技", "科技"),
            Category("财经", "财经"),
            Category("体育", "体育"),
            Category("娱乐", "娱乐"),
            Category("教育", "教育"),
            Category("军事", "军事"),
            Category("健康", "健康")
        )
    }
}
