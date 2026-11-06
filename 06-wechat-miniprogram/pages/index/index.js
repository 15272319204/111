const app = getApp()

const mockMovies = [
  {
    id: 1,
    title: '流浪地球2',
    type: '科幻/冒险',
    director: '郭帆',
    cast: '吴京, 刘德华, 李雪健',
    duration: '173分钟',
    releaseDate: '2023-01-22',
    rating: '9.4',
    description: '太阳即将毁灭，人类在地球表面建造出巨大的推进器，寻找新的家园。然而宇宙之路危机四伏，为了拯救地球，流浪地球时代的年轻人再次挺身而出。',
    poster: 'https://picsum.photos/seed/movie1/300/450',
    showTimes: ['10:00', '13:30', '16:00', '19:30', '22:00'],
    isFavorite: false
  },
  {
    id: 2,
    title: '满江红',
    type: '悬疑/历史',
    director: '张艺谋',
    cast: '沈腾, 易烊千玺, 张译',
    duration: '159分钟',
    releaseDate: '2023-01-22',
    rating: '8.5',
    description: '南宋绍兴年间，岳飞死后四年，秦桧率兵与金国会谈。会谈前夜，金国使者死在宰相驻地，所携密信也不翼而飞。一个小兵与几位义士被裹挟进巨大阴谋之中。',
    poster: 'https://picsum.photos/seed/movie2/300/450',
    showTimes: ['09:30', '12:00', '14:30', '17:00', '20:00'],
    isFavorite: false
  },
  {
    id: 3,
    title: '长津湖',
    type: '战争/历史',
    director: '陈凯歌, 徐克, 林超贤',
    cast: '吴京, 易烊千玺, 段奕宏',
    duration: '176分钟',
    releaseDate: '2021-09-30',
    rating: '9.3',
    description: '讲述了在极寒严酷环境下，中国人民志愿军东线作战部队凭着钢铁意志和英勇无畏的战斗精神，扭转战场态势，为长津湖战役胜利作出重要贡献的故事。',
    poster: 'https://picsum.photos/seed/movie3/300/450',
    showTimes: ['10:30', '14:00', '17:30', '21:00'],
    isFavorite: false
  },
  {
    id: 4,
    title: '你好，李焕英',
    type: '喜剧/剧情',
    director: '贾玲',
    cast: '贾玲, 张小斐, 沈腾',
    duration: '128分钟',
    releaseDate: '2021-02-12',
    rating: '9.2',
    description: '2001年的某一天，刚刚考上大学的贾晓玲在一天之内经历了人生的大起大落。在情绪失控中，意外穿越回到了八十年代，与 20 年前正值青春的母亲李焕英相遇。',
    poster: 'https://picsum.photos/seed/movie4/300/450',
    showTimes: ['11:00', '13:30', '16:00', '18:30', '21:00'],
    isFavorite: false
  },
  {
    id: 5,
    title: '孤注一掷',
    type: '犯罪/剧情',
    director: '申奥',
    cast: '张艺兴, 金晨, 咏梅',
    duration: '130分钟',
    releaseDate: '2023-08-08',
    rating: '8.7',
    description: '讲述了一个程序员和一个模特被海外高薪工作吸引，最终陷入东南亚诈骗工厂的故事。展现了诈骗行业内部的运作机制和受害者的悲惨遭遇。',
    poster: 'https://picsum.photos/seed/movie5/300/450',
    showTimes: ['10:00', '12:30', '15:00', '17:30', '20:00'],
    isFavorite: false
  },
  {
    id: 6,
    title: '消失的她',
    type: '悬疑/犯罪',
    director: '崔睿, 刘翔',
    cast: '朱一龙, 倪妮, 文咏珊',
    duration: '121分钟',
    releaseDate: '2023-06-22',
    rating: '8.8',
    description: '何非的妻子李木子在结婚周年旅行中离奇失踪，在何非苦寻无果之时，妻子突然现身。但何非却坚称眼前的女人并非妻子。随着真相层层揭开，一场精心策划的阴谋浮出水面。',
    poster: 'https://picsum.photos/seed/movie6/300/450',
    showTimes: ['09:00', '11:30', '14:00', '16:30', '19:00', '21:30'],
    isFavorite: false
  }
]

Page({
  data: {
    movies: [],
    hotMovies: [],
    filteredMovies: [],
    banners: [
      {
        id: 1,
        image: 'https://picsum.photos/seed/banner1/600/300',
        title: '暑期大片来袭',
        desc: '精彩影片等你来看'
      },
      {
        id: 2,
        image: 'https://picsum.photos/seed/banner2/600/300',
        title: '科幻巨制震撼上映',
        desc: '沉浸式观影体验'
      },
      {
        id: 3,
        image: 'https://picsum.photos/seed/banner3/600/300',
        title: '浪漫爱情季',
        desc: '甜蜜相约影院'
      }
    ],
    currentFilter: '',
    showFilterBar: false,
    favoriteIds: []
  },

  onLoad: function () {
    this.loadMovies()
  },

  onShow: function () {
    this.loadMovies()
  },

  loadMovies: function () {
    wx.showLoading({
      title: '加载中...'
    })

    wx.request({
      url: `${app.globalData.baseUrl}/api/movies`,
      method: 'GET',
      success: (res) => {
        wx.hideLoading()
        if (res.data && res.data.code === 200) {
          const movies = res.data.data
          this.setData({
            movies: movies,
            hotMovies: movies.slice(0, 5),
            filteredMovies: movies
          })
          this.checkFavorites()
        } else {
          this.loadMockData()
        }
      },
      fail: () => {
        wx.hideLoading()
        this.loadMockData()
      }
    })
  },

  loadMockData: function () {
    const favoriteIds = this.data.favoriteIds
    const movies = mockMovies.map(movie => ({
      ...movie,
      isFavorite: favoriteIds.includes(movie.id)
    }))
    this.setData({
      movies: movies,
      hotMovies: movies.slice(0, 5),
      filteredMovies: movies
    })
  },

  checkFavorites: function () {
    wx.request({
      url: `${app.globalData.baseUrl}/api/favorites`,
      method: 'GET',
      success: (res) => {
        if (res.data && res.data.code === 200) {
          const favoriteIds = res.data.data.map(item => item.id)
          this.setData({ favoriteIds })
          const updatedMovies = this.data.movies.map(movie => ({
            ...movie,
            isFavorite: favoriteIds.includes(movie.id)
          }))
          this.setData({
            movies: updatedMovies,
            filteredMovies: updatedMovies,
            hotMovies: updatedMovies.slice(0, 5)
          })
        }
      },
      fail: () => {
        console.log('使用本地收藏数据')
      }
    })
  },

  goToDetail: function (e) {
    const id = e.currentTarget.dataset.id
    wx.navigateTo({
      url: `/pages/detail/detail?id=${id}`
    })
  },

  toggleFavoriteMini: function (e) {
    const movieId = e.currentTarget.dataset.id
    const movie = this.data.movies.find(m => m.id === movieId)
    const isFavorite = movie && movie.isFavorite
    const favoriteIds = this.data.favoriteIds

    if (isFavorite) {
      const newFavoriteIds = favoriteIds.filter(id => id !== movieId)
      this.setData({ favoriteIds: newFavoriteIds })
      wx.showToast({ title: '取消收藏', icon: 'none' })
    } else {
      const newFavoriteIds = [...favoriteIds, movieId]
      this.setData({ favoriteIds: newFavoriteIds })
      wx.showToast({ title: '收藏成功', icon: 'none' })
    }

    const updatedMovies = this.data.movies.map(m => ({
      ...m,
      isFavorite: m.id === movieId ? !isFavorite : m.isFavorite
    }))
    this.setData({
      movies: updatedMovies,
      filteredMovies: updatedMovies,
      hotMovies: updatedMovies.slice(0, 5)
    })

    wx.request({
      url: isFavorite 
        ? `${app.globalData.baseUrl}/api/favorites/remove`
        : `${app.globalData.baseUrl}/api/favorites/add`,
      method: 'POST',
      data: { movieId },
      success: () => {},
      fail: () => {}
    })
  },

  showFilter: function () {
    this.setData({
      showFilterBar: !this.data.showFilterBar
    })
  },

  setFilter: function (e) {
    const filter = e.currentTarget.dataset.filter
    this.setData({
      currentFilter: filter,
      showFilterBar: false
    })

    if (filter === '') {
      this.setData({
        filteredMovies: this.data.movies
      })
    } else {
      const filtered = this.data.movies.filter(movie => movie.type === filter)
      this.setData({
        filteredMovies: filtered
      })
    }
  },

  seeMoreHot: function () {
    this.setData({
      filteredMovies: this.data.hotMovies,
      currentFilter: '',
      showFilterBar: false
    })
  },

  goToNotification: function () {
    wx.showToast({
      title: '暂无新通知',
      icon: 'none'
    })
  }
})