package com.example.myapplication.data.remote

import retrofit2.Response
import retrofit2.http.GET
import retrofit2.http.Query

interface NewsApiService {

    @GET("109-34")
    suspend fun getChannels(): Response<ShowApiResponse<ChannelListBody>>

    @GET("109-35")
    suspend fun getNewsList(
        @Query("channelId") channelId: String? = null,
        @Query("channelName") channelName: String? = null,
        @Query("title") title: String? = null,
        @Query("page") page: Int = 1,
        @Query("maxResult") maxResult: Int = 20,
        @Query("id") id: String? = null
    ): Response<ShowApiResponse<NewsListBody>>
}
