package com.example.myapplication

import android.app.Application
import com.example.myapplication.data.local.AppDatabase
import com.example.myapplication.data.remote.RetrofitClient
import com.example.myapplication.data.repository.NewsRepository

class NewsApplication : Application() {

    lateinit var database: AppDatabase
        private set

    lateinit var repository: NewsRepository
        private set

    override fun onCreate() {
        super.onCreate()
        instance = this
        database = AppDatabase.getDatabase(this)
        repository = NewsRepository(database, RetrofitClient.apiService)
    }

    companion object {
        lateinit var instance: NewsApplication
            private set
    }
}
