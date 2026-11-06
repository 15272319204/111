const express = require('express')
const cors = require('cors')

const app = express()
const port = 3000

app.use(cors())
app.use(express.json())

const movies = [
  {
    id: 1,
    title: '流浪地球2',
    type: '科幻/冒险',
    director: '郭帆',
    cast: '吴京, 刘德华, 李雪健',
    duration: '173分钟',
    releaseDate: '2023-01-22',
    rating: '9.4',
    description: '太阳即将毁灭，人类在地球表面建造出巨大的推进器，寻找新的家园。然而宇宙之路危机四伏，为了拯救地球，流浪地球时代的年轻人再次挺身而出。',
    poster: 'https://picsum.photos/seed/movie1/300/450',
    showTimes: ['10:00', '13:30', '16:00', '19:30', '22:00']
  },
  {
    id: 2,
    title: '满江红',
    type: '悬疑/历史',
    director: '张艺谋',
    cast: '沈腾, 易烊千玺, 张译',
    duration: '159分钟',
    releaseDate: '2023-01-22',
    rating: '8.5',
    description: '南宋绍兴年间，岳飞死后四年，秦桧率兵与金国会谈。会谈前夜，金国使者死在宰相驻地，所携密信也不翼而飞。一个小兵与几位义士被裹挟进巨大阴谋之中。',
    poster: 'https://picsum.photos/seed/movie2/300/450',
    showTimes: ['09:30', '12:00', '14:30', '17:00', '20:00']
  },
  {
    id: 3,
    title: '长津湖',
    type: '战争/历史',
    director: '陈凯歌, 徐克, 林超贤',
    cast: '吴京, 易烊千玺, 段奕宏',
    duration: '176分钟',
    releaseDate: '2021-09-30',
    rating: '9.3',
    description: '讲述了在极寒严酷环境下，中国人民志愿军东线作战部队凭着钢铁意志和英勇无畏的战斗精神，扭转战场态势，为长津湖战役胜利作出重要贡献的故事。',
    poster: 'https://picsum.photos/seed/movie3/300/450',
    showTimes: ['10:30', '14:00', '17:30', '21:00']
  },
  {
    id: 4,
    title: '你好，李焕英',
    type: '喜剧/剧情',
    director: '贾玲',
    cast: '贾玲, 张小斐, 沈腾',
    duration: '128分钟',
    releaseDate: '2021-02-12',
    rating: '9.2',
    description: '2001年的某一天，刚刚考上大学的贾晓玲在一天之内经历了人生的大起大落。在情绪失控中，意外穿越回到了八十年代，与 20 年前正值青春的母亲李焕英相遇。',
    poster: 'https://picsum.photos/seed/movie4/300/450',
    showTimes: ['11:00', '13:30', '16:00', '18:30', '21:00']
  },
  {
    id: 5,
    title: '孤注一掷',
    type: '犯罪/剧情',
    director: '申奥',
    cast: '张艺兴, 金晨, 咏梅',
    duration: '130分钟',
    releaseDate: '2023-08-08',
    rating: '8.7',
    description: '讲述了一个程序员和一个模特被海外高薪工作吸引，最终陷入东南亚诈骗工厂的故事。展现了诈骗行业内部的运作机制和受害者的悲惨遭遇。',
    poster: 'https://picsum.photos/seed/movie5/300/450',
    showTimes: ['10:00', '12:30', '15:00', '17:30', '20:00']
  },
  {
    id: 6,
    title: '消失的她',
    type: '悬疑/犯罪',
    director: '崔睿, 刘翔',
    cast: '朱一龙, 倪妮, 文咏珊',
    duration: '121分钟',
    releaseDate: '2023-06-22',
    rating: '8.8',
    description: '何非的妻子李木子在结婚周年旅行中离奇失踪，在何非苦寻无果之时，妻子突然现身。但何非却坚称眼前的女人并非妻子。随着真相层层揭开，一场精心策划的阴谋浮出水面。',
    poster: 'https://picsum.photos/seed/movie6/300/450',
    showTimes: ['09:00', '11:30', '14:00', '16:30', '19:00', '21:30']
  }
]

let favorites = []

const users = [
  { id: 1, username: 'user1', password: '123456', name: '张三', isAdmin: false },
  { id: 2, username: 'user2', password: '123456', name: '李四', isAdmin: false },
  { id: 3, username: 'admin', password: 'admin123', name: '管理员', isAdmin: true }
]

const tickets = [
  {
    id: 1,
    movieTitle: '流浪地球2',
    movieType: '科幻/冒险',
    poster: 'https://picsum.photos/seed/movie1/300/450',
    showTime: '2024-01-15 19:30',
    seat: '3排5座',
    price: 30,
    status: 'unused'
  },
  {
    id: 2,
    movieTitle: '满江红',
    movieType: '悬疑/历史',
    poster: 'https://picsum.photos/seed/movie2/300/450',
    showTime: '2024-01-14 14:00',
    seat: '5排8座',
    price: 35,
    status: 'used'
  },
  {
    id: 3,
    movieTitle: '长津湖',
    movieType: '战争/历史',
    poster: 'https://picsum.photos/seed/movie3/300/450',
    showTime: '2024-01-13 20:00',
    seat: '8排12座',
    price: 28,
    status: 'used'
  }
]

app.post('/api/login', (req, res) => {
  const { username, password, type } = req.body
  
  const user = users.find(u => u.username === username && u.password === password)
  
  if (user) {
    if (type === 'admin' && user.isAdmin) {
      res.json({
        code: 200,
        message: '管理员登录成功',
        data: { name: user.name, isAdmin: true }
      })
    } else if (type === 'user' && !user.isAdmin) {
      res.json({
        code: 200,
        message: '登录成功',
        data: { name: user.name, isAdmin: false }
      })
    } else {
      res.json({
        code: 401,
        message: '账号类型不匹配',
        data: null
      })
    }
  } else {
    res.json({
      code: 401,
      message: '账号或密码错误',
      data: null
    })
  }
})

app.get('/api/tickets', (req, res) => {
  res.json({
    code: 200,
    message: 'success',
    data: tickets
  })
})

app.post('/api/tickets/buy', (req, res) => {
  const { movieTitle, movieType, poster, showTime, seat, price } = req.body
  
  const newTicket = {
    id: tickets.length + 1,
    movieTitle,
    movieType,
    poster,
    showTime,
    seat,
    price,
    status: 'unused'
  }
  
  tickets.push(newTicket)
  
  res.json({
    code: 200,
    message: '购票成功',
    data: newTicket
  })
})

app.get('/api/movies', (req, res) => {
  res.json({
    code: 200,
    message: 'success',
    data: movies
  })
})

app.get('/api/movies/:id', (req, res) => {
  const id = parseInt(req.params.id)
  const movie = movies.find(m => m.id === id)
  if (movie) {
    res.json({
      code: 200,
      message: 'success',
      data: movie
    })
  } else {
    res.json({
      code: 404,
      message: '影片不存在',
      data: null
    })
  }
})

app.get('/api/favorites', (req, res) => {
  const favoriteMovies = favorites.map(favId => movies.find(m => m.id === favId)).filter(Boolean)
  res.json({
    code: 200,
    message: 'success',
    data: favoriteMovies
  })
})

app.get('/api/favorites/check', (req, res) => {
  const movieId = parseInt(req.query.movieId)
  const isFavorite = favorites.includes(movieId)
  res.json({
    code: 200,
    message: 'success',
    data: isFavorite
  })
})

app.post('/api/favorites/add', (req, res) => {
  const movieId = parseInt(req.body.movieId)
  if (!favorites.includes(movieId)) {
    favorites.push(movieId)
    res.json({
      code: 200,
      message: '收藏成功',
      data: true
    })
  } else {
    res.json({
      code: 200,
      message: '已收藏',
      data: true
    })
  }
})

app.post('/api/favorites/remove', (req, res) => {
  const movieId = parseInt(req.body.movieId)
  const index = favorites.indexOf(movieId)
  if (index > -1) {
    favorites.splice(index, 1)
    res.json({
      code: 200,
      message: '取消收藏成功',
      data: true
    })
  } else {
    res.json({
      code: 200,
      message: '未收藏',
      data: false
    })
  }
})

app.post('/api/favorites/clear', (req, res) => {
  favorites = []
  res.json({
    code: 200,
    message: '清空成功',
    data: true
  })
})

app.listen(port, () => {
  console.log(`服务器运行在 http://localhost:${port}`)
})