const app = getApp()

Page({
  data: {
    stats: {
      movies: 6,
      tickets: 128,
      users: 256
    },
    recentOrders: [
      {
        id: 1,
        movieTitle: '流浪地球2',
        poster: 'https://picsum.photos/seed/movie1/300/450',
        showTime: '2024-01-15 19:30',
        seat: '3排5座',
        price: 30,
        status: 'unused'
      },
      {
        id: 2,
        movieTitle: '满江红',
        poster: 'https://picsum.photos/seed/movie2/300/450',
        showTime: '2024-01-14 14:00',
        seat: '5排8座',
        price: 35,
        status: 'used'
      },
      {
        id: 3,
        movieTitle: '长津湖',
        poster: 'https://picsum.photos/seed/movie3/300/450',
        showTime: '2024-01-13 20:00',
        seat: '8排12座',
        price: 28,
        status: 'used'
      }
    ]
  },

  onLoad: function () {
    this.checkAdmin()
    this.loadData()
  },

  checkAdmin: function () {
    const userInfo = wx.getStorageSync('userInfo')
    if (!userInfo || !userInfo.isAdmin) {
      wx.showModal({
        title: '权限不足',
        content: '只有管理员才能访问此页面',
        showCancel: false,
        success: () => {
          wx.switchTab({ url: '/pages/profile/profile' })
        }
      })
    }
  },

  loadData: function () {
    wx.request({
      url: `${app.globalData.baseUrl}/api/tickets`,
      success: (res) => {
        if (res.data && res.data.code === 200) {
          const tickets = res.data.data
          this.setData({
            recentOrders: tickets.slice(0, 3),
            stats: {
              movies: 6,
              tickets: tickets.length,
              users: 256
            }
          })
        }
      },
      fail: () => {
        console.log('使用本地数据')
      }
    })
  },

  logout: function () {
    wx.showModal({
      title: '提示',
      content: '确定要退出登录吗？',
      success: (res) => {
        if (res.confirm) {
          wx.removeStorageSync('userInfo')
          wx.showToast({ title: '已退出登录', icon: 'none' })
          setTimeout(() => {
            wx.switchTab({ url: '/pages/profile/profile' })
          }, 1000)
        }
      }
    })
  },

  goToMovieManagement: function () {
    wx.showToast({ title: '影片管理功能开发中', icon: 'none' })
  },

  goToOrderManagement: function () {
    wx.showToast({ title: '订单管理功能开发中', icon: 'none' })
  },

  goToUserManagement: function () {
    wx.showToast({ title: '用户管理功能开发中', icon: 'none' })
  },

  goToDataStatistics: function () {
    wx.showToast({ title: '数据统计功能开发中', icon: 'none' })
  },

  goToCouponManagement: function () {
    wx.showToast({ title: '优惠券管理功能开发中', icon: 'none' })
  },

  goToSystemSettings: function () {
    wx.showToast({ title: '系统设置功能开发中', icon: 'none' })
  },

  goBack: function () {
    wx.switchTab({ url: '/pages/profile/profile' })
  }
})
