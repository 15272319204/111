package com.example.myapplication.data.remote

data class ShowApiResponse<T>(
    val showapi_res_code: Int = -1,
    val showapi_res_error: String = "",
    val showapi_res_body: T? = null
)

data class ChannelListBody(
    val ret_code: Int = 0,
    val totalNum: Int = 0,
    val channelList: List<ChannelItem> = emptyList()
)

data class ChannelItem(
    val channelId: String = "",
    val name: String = ""
)

data class NewsListBody(
    val ret_code: Int = 0,
    val pagebean: NewsPageBean? = null
)

data class NewsPageBean(
    val allNum: Int = 0,
    val allPages: Int = 0,
    val currentPage: Int = 1,
    val maxResult: Int = 20,
    val contentlist: List<NewsItem> = emptyList()
)

data class NewsItem(
    val nid: String = "",
    val title: String = "",
    val link: String = "",
    val pubDate: String = "",
    val source: String = "",
    val channelId: String = "",
    val channelName: String = "",
    val desc: String = "",
    val imageurls: List<NewsImage> = emptyList()
)

data class NewsImage(
    val url: String = ""
)
