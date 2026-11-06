App({
  globalData: {
    userInfo: null,
    baseUrl: 'http://localhost:3000'
  },

  onLaunch: function () {
    console.log('校园影院小程序启动')
  },

  onShow: function () {
    console.log('小程序显示')
  },

  onHide: function () {
    console.log('小程序隐藏')
  }
})