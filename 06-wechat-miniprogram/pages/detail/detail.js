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
    showTimes: ['10:00', '13:30', '16:00', '19:30', '22:00']
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
    showTimes: ['09:30', '12:00', '14:30', '17:00', '20:00']
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
    showTimes: ['10:30', '14:00', '17:30', '21:00']
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
    showTimes: ['11:00', '13:30', '16:00', '18:30', '21:00']
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
    showTimes: ['10:00', '12:30', '15:00', '17:30', '20:00']
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
    showTimes: ['09:00', '11:30', '14:00', '16:30', '19:00', '21:30']
  }
]

Page({
  data: {
    movie: null,
    isFavorite: false,
    selectedTime: '',
    selectedCombo: '',
    ticketPrice: 30,
    totalPrice: 30,
    combos: [
      { id: 1, icon: '🍿', name: '单人套餐', desc: '中桶爆米花+可乐', originalPrice: 35, price: 28 },
      { id: 2, icon: '🥤', name: '双人套餐', desc: '大桶爆米花+2杯可乐', originalPrice: 60, price: 48 },
      { id: 3, icon: '🍭', name: '家庭套餐', desc: '超大桶爆米花+3杯可乐+薯条', originalPrice: 90, price: 72 }
    ]
  },

  onLoad: function (options) {
    const id = parseInt(options.id) || 1
    this.loadMovieDetail(id)
  },

  loadMovieDetail: function (id) {
    wx.showLoading({
      title: '加载中...'
    })

    wx.request({
      url: `${app.globalData.baseUrl}/api/movies/${id}`,
      method: 'GET',
      success: (res) => {
        wx.hideLoading()
        if (res.data && res.data.code === 200) {
          this.setData({
            movie: res.data.data,
            selectedTime: res.data.data.showTimes[0] || ''
          })
          this.checkFavorite(id)
        } else {
          this.loadMockMovie(id)
        }
      },
      fail: () => {
        wx.hideLoading()
        this.loadMockMovie(id)
      }
    })
  },

  loadMockMovie: function (id) {
    const movie = mockMovies.find(m => m.id === id) || mockMovies[0]
    this.setData({
      movie: movie,
      selectedTime: movie.showTimes[0] || ''
    })
    this.checkFavorite(id)
  },

  checkFavorite: function (movieId) {
    // 先尝试从服务器检查
    wx.request({
      url: `${app.globalData.baseUrl}/api/favorites/check`,
      method: 'GET',
      data: { movieId },
      success: (res) => {
        if (res.data && res.data.code === 200) {
          this.setData({
            isFavorite: res.data.data
          })
        }
      },
      fail: () => {
        // 服务器失败时，使用本地存储检查
        const favorites = wx.getStorageSync('favorites') || []
        const isFavorite = favorites.includes(movieId)
        this.setData({ isFavorite })
      }
    })
  },

  toggleFavorite: function () {
    const movieId = this.data.movie.id
    const isFavorite = !this.data.isFavorite
    
    // 更新本地存储
    let favorites = wx.getStorageSync('favorites') || []
    if (isFavorite) {
      if (!favorites.includes(movieId)) {
        favorites.push(movieId)
      }
    } else {
      favorites = favorites.filter(id => id !== movieId)
    }
    wx.setStorageSync('favorites', favorites)
    
    this.setData({ isFavorite })
    wx.showToast({
      title: isFavorite ? '收藏成功' : '取消收藏',
      icon: 'none'
    })

    const url = isFavorite 
      ? `${app.globalData.baseUrl}/api/favorites/add`
      : `${app.globalData.baseUrl}/api/favorites/remove`

    wx.request({
      url: url,
      method: 'POST',
      data: { movieId },
      success: () => {},
      fail: () => {}
    })
  },

  selectTime: function (e) {
    const time = e.currentTarget.dataset.time
    this.setData({
      selectedTime: time
    })
  },

  selectCombo: function (e) {
    const comboId = parseInt(e.currentTarget.dataset.id)
    let totalPrice = this.data.ticketPrice
    
    if (this.data.selectedCombo === comboId) {
      this.setData({
        selectedCombo: '',
        totalPrice: totalPrice
      })
    } else {
      const combo = this.data.combos.find(c => c.id === comboId)
      if (combo) {
        totalPrice += combo.price
      }
      this.setData({
        selectedCombo: comboId,
        totalPrice: totalPrice
      })
    }
  },

  playTrailer: function () {
    wx.showModal({
      title: '播放预告',
      content: '即将播放影片预告片',
      confirmText: '确定',
      confirmColor: '#e53935',
      success: (res) => {
        if (res.confirm) {
          wx.showToast({
            title: '预告播放中',
            icon: 'none'
          })
        }
      }
    })
  },

  shareMovie: function () {
    wx.showActionSheet({
      itemList: ['分享给好友', '生成海报', '复制链接'],
      success: (res) => {
        const actions = ['分享给好友', '生成海报', '复制链接']
        wx.showToast({
          title: `${actions[res.tapIndex]}功能开发中`,
          icon: 'none'
        })
      }
    })
  },

  goToBuy: function () {
    // 检查是否登录
    const userInfo = wx.getStorageSync('userInfo')
    if (!userInfo) {
      wx.showModal({
        title: '提示',
        content: '购票需要先登录，请先登录账号',
        showCancel: false,
        confirmText: '去登录',
        confirmColor: '#e53935',
        success: () => {
          wx.switchTab({ url: '/pages/profile/profile' })
        }
      })
      return
    }

    if (!this.data.selectedTime) {
      wx.showToast({
        title: '请选择放映时间',
        icon: 'none'
      })
      return
    }

    const movieStr = encodeURIComponent(JSON.stringify(this.data.movie))
    wx.navigateTo({
      url: `/pages/confirm/confirm?movie=${movieStr}`
    })
  },

  goBack: function () {
    wx.navigateBack()
  }
})