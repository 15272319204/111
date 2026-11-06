const app = getApp()

const mockTickets = [
  {
    id: 1,
    movieTitle: '流浪地球 2',
    movieType: '科幻/冒险',
    poster: 'https://picsum.photos/seed/movie1/300/450',
    showTime: '2024-01-15 19:30',
    seat: '3 排 5 座',
    price: 30,
    status: 'unused'
  },
  {
    id: 2,
    movieTitle: '满江红',
    movieType: '悬疑/历史',
    poster: 'https://picsum.photos/seed/movie2/300/450',
    showTime: '2024-01-14 14:00',
    seat: '5 排 8 座',
    price: 35,
    status: 'used'
  }
]

Page({
  data: {
    ticketList: [],
    localTickets: []
  },

  onLoad: function () {
    this.loadTickets()
  },

  onShow: function () {
    this.loadTickets()
  },

  loadTickets: function () {
    wx.showLoading({ title: '加载中...' })

    wx.request({
      url: app.globalData.baseUrl + '/api/tickets',
      method: 'GET',
      success: (res) => {
        wx.hideLoading()
        if (res.data && res.data.code === 200) {
          this.setData({ ticketList: res.data.data })
        } else {
          this.loadLocalTickets()
        }
      },
      fail: () => {
        wx.hideLoading()
        this.loadLocalTickets()
      }
    })
  },

  loadLocalTickets: function () {
    const localTickets = wx.getStorageSync('tickets') || mockTickets
    this.setData({ 
      ticketList: localTickets,
      localTickets
    })
  },

  useTicket: function (e) {
    const ticketId = e.currentTarget.dataset.id
    
    wx.showModal({
      title: '核销票券',
      content: '确定要核销这张票券吗？',
      success: (res) => {
        if (res.confirm) {
          // 更新本地存储
          let localTickets = wx.getStorageSync('tickets') || []
          const ticket = localTickets.find(t => t.id === ticketId)
          if (ticket) {
            ticket.status = 'used'
            wx.setStorageSync('tickets', localTickets)
          }
          
          // 更新页面数据
          const ticketList = this.data.ticketList.map(t => {
            if (t.id === ticketId) {
              return { ...t, status: 'used' }
            }
            return t
          })
          this.setData({ ticketList, localTickets })
          
          wx.showToast({
            title: '核销成功',
            icon: 'success'
          })
          
          // 同步到服务器
          wx.request({
            url: app.globalData.baseUrl + '/api/tickets/use',
            method: 'POST',
            data: { ticketId },
            success: () => {},
            fail: () => {}
          })
        }
      }
    })
  },

  goToDetail: function (e) {
    const ticketId = e.currentTarget.dataset.id
    wx.navigateTo({
      url: `/pages/ticket-detail/ticket-detail?id=${ticketId}`
    })
  },

  goToHome: function () {
    wx.switchTab({ url: '/pages/index/index' })
  }
})