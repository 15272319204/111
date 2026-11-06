const app = getApp()

const defaultUsers = [
  { id: 1, username: 'user1', password: '123456', name: '张三', isAdmin: false },
  { id: 2, username: 'user2', password: '123456', name: '李四', isAdmin: false },
  { id: 3, username: 'admin', password: 'admin123', name: '管理员', isAdmin: true }
]

Page({
  data: {
    isLoggedIn: false,
    authMode: 'login',
    loginType: 'user',
    userInfo: {
      name: '',
      isAdmin: false
    },
    formData: {
      username: '',
      password: '',
      confirmPassword: ''
    }
  },

  onLoad: function () {
    this.checkLoginStatus()
  },

  onShow: function () {
    this.checkLoginStatus()
  },

  checkLoginStatus: function () {
    const userInfo = wx.getStorageSync('userInfo')
    if (userInfo) {
      this.setData({
        isLoggedIn: true,
        userInfo: userInfo
      })
    }
  },

  setAuthMode: function (e) {
    const mode = e.currentTarget.dataset.mode
    this.setData({
      authMode: mode,
      formData: { username: '', password: '', confirmPassword: '' }
    })
  },

  setLoginType: function (e) {
    const type = e.currentTarget.dataset.type
    this.setData({
      loginType: type,
      formData: { username: '', password: '', confirmPassword: '' }
    })
  },

  handleInput: function (e) {
    const field = e.currentTarget.dataset.field
    const value = e.detail.value
    this.setData({
      [`formData.${field}`]: value
    })
  },

  login: function () {
    const username = (this.data.formData.username || '').trim()
    const password = (this.data.formData.password || '').trim()
    const { loginType } = this.data

    if (!username || !password) {
      wx.showToast({ title: '请填写账号和密码', icon: 'none' })
      return
    }

    const users = JSON.parse(JSON.stringify(defaultUsers))
    const user = users.find(u => u.username === username && u.password === password)

    if (user) {
      if (loginType === 'admin' && !user.isAdmin) {
        wx.showToast({ title: '该账号不是管理员', icon: 'none' })
        return
      }
      if (loginType === 'user' && user.isAdmin) {
        wx.showToast({ title: '请使用管理员登录', icon: 'none' })
        return
      }

      const userInfo = {
        name: user.name,
        isAdmin: user.isAdmin
      }
      wx.setStorageSync('userInfo', userInfo)
      this.setData({
        isLoggedIn: true,
        userInfo: userInfo
      })
      wx.showToast({
        title: user.isAdmin ? '管理员登录成功' : '登录成功',
        icon: 'success'
      })
    } else {
      wx.showModal({
        title: '登录失败',
        content: '账号或密码错误。\n\n可用账号：\n用户1: user1 / 123456\n用户2: user2 / 123456\n管理员: admin / admin123',
        showCancel: false
      })
    }
  },

  register: function () {
    const username = (this.data.formData.username || '').trim()
    const password = (this.data.formData.password || '').trim()
    const confirmPassword = (this.data.formData.confirmPassword || '').trim()

    if (!username || !password) {
      wx.showToast({ title: '请填写账号和密码', icon: 'none' })
      return
    }

    if (password.length < 6) {
      wx.showToast({ title: '密码至少6位', icon: 'none' })
      return
    }

    if (password !== confirmPassword) {
      wx.showToast({ title: '两次密码不一致', icon: 'none' })
      return
    }

    let users = wx.getStorageSync('users') || []
    if (users.find(u => u.username === username)) {
      wx.showToast({ title: '该账号已存在', icon: 'none' })
      return
    }

    const newUser = {
      id: users.length + 1,
      username: username,
      password: password,
      name: username,
      isAdmin: false
    }
    users.push(newUser)
    wx.setStorageSync('users', users)

    wx.showToast({ title: '注册成功', icon: 'success' })
    this.setData({
      authMode: 'login',
      formData: { username: '', password: '', confirmPassword: '' }
    })
  },

  logout: function () {
    wx.showModal({
      title: '提示',
      content: '确定要退出登录吗？',
      success: (res) => {
        if (res.confirm) {
          wx.removeStorageSync('userInfo')
          this.setData({
            isLoggedIn: false,
            userInfo: { name: '', isAdmin: false }
          })
          wx.showToast({ title: '已退出登录', icon: 'none' })
        }
      }
    })
  },

  goToRegister: function () {
    this.setAuthMode({ currentTarget: { dataset: { mode: 'register' } } })
  },

  forgotPassword: function () {
    wx.showToast({ title: '找回密码功能开发中', icon: 'none' })
  },

  goToOrder: function () {
    wx.switchTab({ url: '/pages/tickets/tickets' })
  },

  goToCoupons: function () {
    wx.navigateTo({ url: '/pages/coupons/coupons' })
  },

  goToHistory: function () {
    wx.navigateTo({ url: '/pages/history/history' })
  },

  goToSettings: function () {
    wx.navigateTo({ url: '/pages/settings/settings' })
  },

  goToAdmin: function () {
    wx.navigateTo({ url: '/pages/admin/admin' })
  }
})
