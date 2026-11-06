Component({
  data: {
    tabList: [
      { pagePath: '/pages/index/index', text: '首页' },
      { pagePath: '/pages/favorites/favorites', text: '收藏' },
      { pagePath: '/pages/tickets/tickets', text: '购票' },
      { pagePath: '/pages/profile/profile', text: '我的' }
    ],
    selectedIndex: 0
  },

  attached: function () {
    this.updateSelectedIndex()
  },

  methods: {
    updateSelectedIndex: function () {
      const pages = getCurrentPages()
      if (pages.length > 0) {
        const currentPage = pages[pages.length - 1]
        const currentPath = '/' + currentPage.route
        const index = this.data.tabList.findIndex(item => item.pagePath === currentPath)
        if (index !== -1) {
          this.setData({ selectedIndex: index })
        }
      }
    },

    switchTab: function (e) {
      const path = e.currentTarget.dataset.path
      wx.switchTab({ url: path })
    }
  }
})