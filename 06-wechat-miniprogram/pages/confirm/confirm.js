const app = getApp()

const mockCombos = [
  { id: 1, name: '单人套餐', desc: '中桶爆米花+可乐', icon: '🍿', originalPrice: 35, price: 28 },
  { id: 2, name: '双人套餐', desc: '大桶爆米花+2杯可乐', icon: '🥤', originalPrice: 60, price: 48 },
  { id: 3, name: '家庭套餐', desc: '超大桶爆米花+3杯可乐+薯条', icon: '🍟', originalPrice: 90, price: 72 }
]

const defaultMovie = {
  title: '流浪地球2',
  type: '科幻/冒险',
  rating: '9.4',
  duration: '173分钟',
  poster: 'https://picsum.photos/seed/movie1/300/450',
  showTimes: ['10:00', '13:30', '16:00', '18:30', '21:00']
}

Page({
  data: {
    movie: {},
    selectedTime: '',
    selectedTimeText: '未选择',
    seats: [],
    selectedSeats: [],
    selectedSeatsText: '请选择座位',
    selectedSeatsCount: 0,
    combos: mockCombos,
    selectedCombo: null,
    selectedComboName: '',
    selectedComboPrice: 0,
    ticketPrice: 30,
    totalPrice: 0
  },

  onLoad: function (options) {
    let movieData = defaultMovie
    if (options && options.movie) {
      try {
        const parsed = JSON.parse(decodeURIComponent(options.movie))
        if (parsed && parsed.title) {
          movieData = parsed
        }
      } catch (e) {
        console.log('解析电影数据失败，使用默认数据')
      }
    }
    this.setData({ 
      movie: movieData,
      selectedTime: movieData.showTimes[0] || '',
      selectedTimeText: movieData.showTimes[0] || '未选择'
    })
    this.initSeats()
  },

  initSeats: function () {
    const rows = ['A', 'B', 'C', 'D', 'E', 'F']
    const cols = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    const seats = []
    
    rows.forEach(row => {
      const rowSeats = cols.map(col => ({
        row,
        col,
        status: Math.random() > 0.3 ? 'available' : 'sold',
        selected: false
      }))
      seats.push(rowSeats)
    })
    
    this.setData({ seats })
  },

  selectTime: function (e) {
    const time = e.currentTarget.dataset.time
    this.setData({ 
      selectedTime: time,
      selectedTimeText: time
    })
    this.calculateTotal()
  },

  selectSeat: function (e) {
    const row = e.currentTarget.dataset.row
    const col = e.currentTarget.dataset.col
    
    const seats = this.data.seats.map(rowSeats => 
      rowSeats.map(seat => {
        if (seat.row === row && seat.col === col) {
          if (seat.status === 'available') {
            seat.selected = !seat.selected
          }
        }
        return seat
      })
    )
    
    this.setData({ seats })
    this.updateSelectedSeats()
  },

  updateSelectedSeats: function () {
    const selectedSeats = []
    this.data.seats.forEach(rowSeats => {
      rowSeats.forEach(seat => {
        if (seat.selected) {
          selectedSeats.push(`${seat.row}${seat.col}`)
        }
      })
    })
    
    const seatsText = selectedSeats.length > 0 ? '已选' + selectedSeats.length + '座' : '请选择座位'
    this.setData({ 
      selectedSeats,
      selectedSeatsText: seatsText,
      selectedSeatsCount: selectedSeats.length
    })
    this.calculateTotal()
  },

  selectCombo: function (e) {
    const combo = e.currentTarget.dataset.combo
    if (this.data.selectedCombo && this.data.selectedCombo.id === combo.id) {
      this.setData({ 
        selectedCombo: null,
        selectedComboName: '',
        selectedComboPrice: 0
      })
    } else {
      this.setData({ 
        selectedCombo: combo,
        selectedComboName: combo.name,
        selectedComboPrice: combo.price
      })
    }
    this.calculateTotal()
  },

  calculateTotal: function () {
    const seatPrice = this.data.selectedSeats.length * this.data.ticketPrice
    const comboPrice = this.data.selectedCombo ? this.data.selectedCombo.price : 0
    this.setData({ totalPrice: seatPrice + comboPrice })
  },

  submitOrder: function () {
    if (!this.data.selectedTime || this.data.selectedSeats.length === 0) {
      wx.showToast({ title: '请选择场次和座位', icon: 'none' })
      return
    }

    wx.showLoading({ title: '提交中...' })

    const orderData = {
      movieTitle: this.data.movie.title,
      movieType: this.data.movie.type,
      poster: this.data.movie.poster,
      showTime: '2024-01-15 ' + this.data.selectedTime,
      seat: this.data.selectedSeats.join(', '),
      price: this.data.totalPrice,
      status: 'unused'
    }

    // 保存到本地存储
    let tickets = wx.getStorageSync('tickets') || []
    const newTicket = {
      id: Date.now(),
      ...orderData
    }
    tickets.unshift(newTicket)
    wx.setStorageSync('tickets', tickets)

    wx.request({
      url: app.globalData.baseUrl + '/api/tickets/buy',
      method: 'POST',
      data: orderData,
      success: (res) => {
        wx.hideLoading()
        if (res.data && res.data.code === 200) {
          wx.showModal({
            title: '购票成功',
            content: '已购买' + this.data.selectedSeats.length + '张票，座位：' + this.data.selectedSeats.join(', '),
            showCancel: false,
            confirmText: '查看订单',
            success: () => {
              wx.switchTab({ url: '/pages/tickets/tickets' })
            }
          })
        } else {
          wx.showToast({ title: '购票成功', icon: 'success' })
          setTimeout(() => {
            wx.switchTab({ url: '/pages/tickets/tickets' })
          }, 1500)
        }
      },
      fail: () => {
        wx.hideLoading()
        wx.showModal({
          title: '购票成功',
          content: '已购买' + this.data.selectedSeats.length + '张票，座位：' + this.data.selectedSeats.join(', '),
          showCancel: false,
          confirmText: '查看订单',
          success: () => {
            wx.switchTab({ url: '/pages/tickets/tickets' })
          }
        })
      }
    })
  },

  goBack: function () {
    wx.navigateBack()
  }
})