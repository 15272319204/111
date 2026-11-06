const app = getApp()

Page({
  data: {
    selectedYear: '',
    selectedMonth: '',
    historyList: [
      {
        id: 1,
        title: '流浪地球2',
        type: '科幻/冒险',
        poster: 'https://picsum.photos/seed/movie1/300/450',
        rating: '9.4',
        watchTime: '2024-01-15 19:30',
        dateGroup: '2024年1月'
      },
      {
        id: 2,
        title: '满江红',
        type: '悬疑/历史',
        poster: 'https://picsum.photos/seed/movie2/300/450',
        rating: '8.5',
        watchTime: '2024-01-14 14:00',
        dateGroup: '2024年1月'
      },
      {
        id: 3,
        title: '长津湖',
        type: '战争/历史',
        poster: 'https://picsum.photos/seed/movie3/300/450',
        rating: '9.3',
        watchTime: '2023-12-20 20:00',
        dateGroup: '2023年12月'
      },
      {
        id: 4,
        title: '你好，李焕英',
        type: '喜剧/剧情',
        poster: 'https://picsum.photos/seed/movie4/300/450',
        rating: '9.2',
        watchTime: '2023-11-10 15:30',
        dateGroup: '2023年11月'
      },
      {
        id: 5,
        title: '孤注一掷',
        type: '犯罪/剧情',
        poster: 'https://picsum.photos/seed/movie5/300/450',
        rating: '8.7',
        watchTime: '2023-11-05 19:00',
        dateGroup: '2023年11月'
      },
      {
        id: 6,
        title: '消失的她',
        type: '悬疑/犯罪',
        poster: 'https://picsum.photos/seed/movie6/300/450',
        rating: '8.8',
        watchTime: '2023-10-01 14:00',
        dateGroup: '2023年10月'
      }
    ],
    groupedHistory: []
  },

  onLoad: function () {
    this.groupByDate()
  },

  groupByDate: function () {
    const { historyList } = this.data
    const groups = {}
    
    historyList.forEach(item => {
      const date = item.dateGroup
      if (!groups[date]) {
        groups[date] = []
      }
      groups[date].push(item)
    })
    
    const grouped = Object.keys(groups).map(date => ({
      date: date,
      movies: groups[date]
    }))
    
    this.setData({
      groupedHistory: grouped
    })
  },

  filterByYear: function () {
    wx.showActionSheet({
      itemList: ['全部年份', '2024年', '2023年', '2022年'],
      success: (res) => {
        const years = ['', '2024', '2023', '2022']
        this.setData({
          selectedYear: years[res.tapIndex]
        })
      }
    })
  },

  filterByMonth: function () {
    wx.showActionSheet({
      itemList: ['全部月份', '1月', '2月', '3月', '4月', '5月', '6月', '7月', '8月', '9月', '10月', '11月', '12月'],
      success: (res) => {
        const months = ['', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12']
        this.setData({
          selectedMonth: months[res.tapIndex]
        })
      }
    })
  },

  clearFilter: function () {
    this.setData({
      selectedYear: '',
      selectedMonth: ''
    })
  },

  goToDetail: function (e) {
    const movieId = e.currentTarget.dataset.movieId
    wx.navigateTo({
      url: `/pages/detail/detail?id=${movieId}`
    })
  },

  goToHome: function () {
    wx.switchTab({
      url: '/pages/index/index'
    })
  },

  clearHistory: function () {
    wx.showModal({
      title: '确认清空',
      content: '确定要清空所有观影历史吗？',
      success: (res) => {
        if (res.confirm) {
          this.setData({
            historyList: [],
            groupedHistory: []
          })
          wx.showToast({ title: '已清空', icon: 'success' })
        }
      }
    })
  }
})
