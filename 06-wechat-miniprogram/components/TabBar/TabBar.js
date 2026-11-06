Component({
  properties: {
    currentIndex: {
      type: Number,
      value: 0
    }
  },

  methods: {
    switchTab(e) {
      const index = parseInt(e.currentTarget.dataset.index)
      if (index === 0) {
        wx.navigateTo({
          url: '/pages/index/index'
        })
      } else {
        wx.navigateTo({
          url: '/pages/favorites/favorites'
        })
      }
    },

    goToSearch() {
      wx.showToast({
        title: '搜索功能',
        icon: 'none'
      })
    }
  }
})