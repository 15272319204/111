const app = getApp()

Page({
  data: {
    settings: {
      messageNotification: true,
      emailNotification: false,
      promotionNotification: true
    }
  },

  onLoad: function () {
    this.loadSettings()
  },

  loadSettings: function () {
    const settings = wx.getStorageSync('settings') || {
      messageNotification: true,
      emailNotification: false,
      promotionNotification: true
    }
    this.setData({
      settings: settings
    })
  },

  toggleSetting: function (e) {
    const key = e.currentTarget.dataset.key
    const settings = this.data.settings
    settings[key] = !settings[key]
    this.setData({
      settings: settings
    })
    wx.setStorageSync('settings', settings)
  },

  goToProfile: function () {
    wx.showToast({ title: '个人资料功能开发中', icon: 'none' })
  },

  goToSecurity: function () {
    wx.showToast({ title: '账号安全功能开发中', icon: 'none' })
  },

  goToPrivacy: function () {
    wx.showToast({ title: '隐私设置功能开发中', icon: 'none' })
  },

  goToLanguage: function () {
    wx.showActionSheet({
      itemList: ['简体中文', '繁体中文', 'English'],
      success: (res) => {
        const languages = ['zh-CN', 'zh-TW', 'en']
        wx.showToast({ title: '语言设置已更新', icon: 'success' })
      }
    })
  },

  goToTheme: function () {
    wx.showActionSheet({
      itemList: ['默认主题', '深色模式', '护眼模式'],
      success: (res) => {
        wx.showToast({ title: '主题设置已更新', icon: 'success' })
      }
    })
  },

  goToCache: function () {
    wx.showModal({
      title: '清除缓存',
      content: '确定要清除所有缓存吗？',
      success: (res) => {
        if (res.confirm) {
          wx.showToast({ title: '缓存已清除', icon: 'success' })
        }
      }
    })
  },

  goToAbout: function () {
    wx.showModal({
      title: '关于我们',
      content: '校园影院 v1.0.0\n\n为您提供优质的校园观影服务',
      showCancel: false
    })
  },

  goToFeedback: function () {
    wx.showToast({ title: '意见反馈功能开发中', icon: 'none' })
  },

  goToHelp: function () {
    wx.showToast({ title: '帮助中心功能开发中', icon: 'none' })
  },

  logout: function () {
    wx.showModal({
      title: '退出登录',
      content: '确定要退出登录吗？',
      success: (res) => {
        if (res.confirm) {
          wx.removeStorageSync('userInfo')
          wx.showToast({ title: '已退出登录', icon: 'success' })
          setTimeout(() => {
            wx.switchTab({ url: '/pages/profile/profile' })
          }, 1000)
        }
      }
    })
  }
})