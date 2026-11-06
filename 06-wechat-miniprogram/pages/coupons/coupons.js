const app = getApp()

Page({
  data: {
    activeTab: 'all',
    coupons: [
      {
        id: 1,
        name: '新人专享券',
        description: '新用户首次购票立减',
        value: 10,
        condition: '满30可用',
        expireDate: '2024-12-31',
        status: 'usable'
      },
      {
        id: 2,
        name: '周末特惠券',
        description: '周末观影享受折扣',
        value: 5,
        condition: '全场通用',
        expireDate: '2024-06-30',
        status: 'usable'
      },
      {
        id: 3,
        name: '会员尊享券',
        description: '会员专享优惠',
        value: 15,
        condition: '满50可用',
        expireDate: '2024-05-15',
        status: 'expired'
      },
      {
        id: 4,
        name: '节日礼包券',
        description: '春节特别优惠',
        value: 20,
        condition: '满60可用',
        expireDate: '2024-02-29',
        status: 'used'
      },
      {
        id: 5,
        name: '学生优惠',
        description: '学生专享折扣',
        value: 8,
        condition: '全场通用',
        expireDate: '2024-12-31',
        status: 'usable'
      }
    ],
    filteredCoupons: []
  },

  onLoad: function () {
    this.filterCoupons()
  },

  setTab: function (e) {
    const tab = e.currentTarget.dataset.tab
    this.setData({
      activeTab: tab
    })
    this.filterCoupons()
  },

  filterCoupons: function () {
    const { activeTab, coupons } = this.data
    let filtered = coupons
    
    if (activeTab === 'usable') {
      filtered = coupons.filter(c => c.status === 'usable')
    } else if (activeTab === 'used') {
      filtered = coupons.filter(c => c.status === 'used')
    } else if (activeTab === 'expired') {
      filtered = coupons.filter(c => c.status === 'expired')
    }
    
    this.setData({
      filteredCoupons: filtered
    })
  },

  getCoupons: function () {
    wx.showModal({
      title: '领取优惠券',
      content: '恭喜您！成功领取以下优惠券：\n\n🎫 满50减10优惠券\n🎫 满30减5优惠券',
      showCancel: false,
      success: () => {
        this.setData({
          coupons: [
            ...this.data.coupons,
            {
              id: Date.now(),
              name: '满50减10',
              description: '限时领取',
              value: 10,
              condition: '满50可用',
              expireDate: '2024-12-31',
              status: 'usable'
            },
            {
              id: Date.now() + 1,
              name: '满30减5',
              description: '限时领取',
              value: 5,
              condition: '满30可用',
              expireDate: '2024-12-31',
              status: 'usable'
            }
          ]
        })
        this.filterCoupons()
        wx.showToast({ title: '领取成功', icon: 'success' })
      }
    })
  }
})
