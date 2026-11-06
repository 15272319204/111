package com.example.myapplication.viewmodel

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import com.example.myapplication.NewsApplication
import com.example.myapplication.data.model.Category
import com.example.myapplication.data.model.NewsArticle
import com.example.myapplication.data.remote.ChannelItem
import com.example.myapplication.data.remote.NetworkResult
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch

class NewsViewModel : ViewModel() {

    private val repository = NewsApplication.instance.repository

    private val _articles = MutableStateFlow<List<NewsArticle>>(emptyList())
    val articles: StateFlow<List<NewsArticle>> = _articles.asStateFlow()

    private val _categories = MutableStateFlow<List<Category>>(Category.DEFAULTS)
    val categories: StateFlow<List<Category>> = _categories.asStateFlow()

    private val _apiChannels = MutableStateFlow<List<ChannelItem>>(emptyList())
    val apiChannels: StateFlow<List<ChannelItem>> = _apiChannels.asStateFlow()

    private val _currentCategory = MutableStateFlow("")
    val currentCategory: StateFlow<String> = _currentCategory.asStateFlow()

    private val _currentChannelId = MutableStateFlow<String?>(null)
    val currentChannelId: StateFlow<String?> = _currentChannelId.asStateFlow()

    private val _isLoading = MutableStateFlow(false)
    val isLoading: StateFlow<Boolean> = _isLoading.asStateFlow()

    private val _errorMessage = MutableStateFlow<String?>(null)
    val errorMessage: StateFlow<String?> = _errorMessage.asStateFlow()

    private val _isRefreshing = MutableStateFlow(false)
    val isRefreshing: StateFlow<Boolean> = _isRefreshing.asStateFlow()

    init {
        seedDemoIfEmpty()
    }

    private fun seedDemoIfEmpty() {
        viewModelScope.launch {
            val count = repository.getNewsCount()
            if (count == 0) {
                seedDemoData()
            }
            loadChannels()
        }
    }

    private fun seedDemoData() {
        viewModelScope.launch {
            val demoNews = listOf(
                NewsArticle(
                    id = 1001,
                    title = "2026年人工智能技术发展新趋势",
                    content = "人工智能技术正在以前所未有的速度发展。2026年，大模型应用进入全面落地阶段，多模态AI成为主流。各大科技公司纷纷推出新一代AI产品，覆盖教育、医疗、金融等多个领域。专家预测，未来五年AI将重塑全球产业格局。\n\n在自然语言处理领域，新一代模型已经能够理解更复杂的上下文，支持超长文本处理。计算机视觉技术也取得重大突破，实时视频分析能力大幅提升。\n\n国内AI产业也蓬勃发展，多家企业推出自主研发的大模型产品，在特定垂直领域展现出强大竞争力。",
                    summary = "AI技术全面落地，多模态模型成为2026年主流趋势",
                    category = "科技",
                    author = "科技日报",
                    sourceUrl = "https://example.com/ai-2026",
                    publishTime = System.currentTimeMillis() - 3600000
                ),
                NewsArticle(
                    id = 1002,
                    title = "全球经济展望：2026年第二季度增长预期上调",
                    content = "国际货币基金组织最新报告显示，2026年全球经济增速预计达到3.5%，较年初预期上调0.2个百分点。亚洲经济体表现强劲，成为全球经济增长的主要驱动力。\n\n报告指出，全球通胀持续回落，主要央行开始逐步放松货币政策。国际贸易复苏势头明显，货物贸易和服务贸易均呈现增长态势。\n\n新兴市场和发展中经济体展现出较强韧性，数字化转型和绿色投资成为新的增长点。",
                    summary = "IMF上调2026年全球经济增长预期至3.5%",
                    category = "财经",
                    author = "经济观察报",
                    sourceUrl = "https://example.com/economy-2026",
                    publishTime = System.currentTimeMillis() - 7200000
                ),
                NewsArticle(
                    id = 1003,
                    title = "中国队在2026世界杯预选赛取得关键胜利",
                    content = "在昨晚进行的世界杯亚洲区预选赛中，中国队以3:1击败对手，取得出线关键三分。上半场比赛中国队率先破门，下半场继续扩大领先优势。\n\n主教练赛后表示，球队整体发挥出色，战术执行到位。这是中国队在本届世预赛中最重要的胜利之一，为争取直接出线名额奠定了坚实基础。\n\n下一场比赛将在一周后进行，中国队将在主场迎战小组头名。",
                    summary = "国足3:1取胜，世界杯出线形势大好",
                    category = "体育",
                    author = "体坛周报",
                    sourceUrl = "https://example.com/football-2026",
                    publishTime = System.currentTimeMillis() - 1800000
                ),
                NewsArticle(
                    id = 1004,
                    title = "教育部发布新课标：编程纳入中小学必修课程",
                    content = "教育部近日发布《义务教育课程方案和课程标准（2026年版）》，明确将编程教育纳入中小学必修课程体系。从小学三年级开始，学生将系统学习计算机科学基础知识。\n\n新课标强调培养学生的计算思维和数字化创新能力，课程内容涵盖编程基础、算法思维、人工智能启蒙等模块。各地学校将配备专业师资和完善的硬件设施。\n\n此举旨在为国家培养面向未来的数字化人才，助力科技自立自强战略。",
                    summary = "编程正式成为中小学必修课，今秋开始实施",
                    category = "教育",
                    author = "中国教育报",
                    sourceUrl = "https://example.com/edu-coding",
                    publishTime = System.currentTimeMillis() - 14400000
                ),
                NewsArticle(
                    id = 1005,
                    title = "新型量子芯片突破：计算速度提升千倍",
                    content = "我国科研团队成功研发出一款新型超导量子芯片，量子比特数量达到1024个，计算速度较上一代产品提升千倍。这一突破标志着中国在量子计算领域跻身世界前列。\n\n研究团队负责人介绍，该芯片采用全新的量子纠错架构，有效降低了量子退相干效应，使得量子计算的稳定性和可靠性大幅提升。\n\n该成果已发表在国际顶级学术期刊上，受到全球科学界的高度关注。量子计算机的商业化应用有望在未来五年内实现。",
                    summary = "中国团队研发出1024量子比特芯片，性能全球领先",
                    category = "科技",
                    author = "科学日报",
                    sourceUrl = "https://example.com/quantum",
                    publishTime = System.currentTimeMillis() - 21600000
                ),
                NewsArticle(
                    id = 1006,
                    title = "夏季健康指南：高温天气如何科学防暑降温",
                    content = "随着夏季高温天气来临，各地气温持续攀升。医疗专家提醒公众注意防暑降温，避免中暑和热射病的发生。\n\n专家建议：避免在中午高温时段外出活动；多饮白开水或淡盐水补充水分；穿着轻薄透气的衣物；注意室内通风降温。\n\n老年人、儿童和慢性病患者是高温天气的高危人群，家人应做好看护。如出现头晕、恶心、胸闷等症状应及时就医。",
                    summary = "高温来袭，收好这份科学防暑指南",
                    category = "健康",
                    author = "健康时报",
                    sourceUrl = "https://example.com/health-summer",
                    publishTime = System.currentTimeMillis() - 43200000
                ),
                NewsArticle(
                    id = 1007,
                    title = "国际形势分析：多极化世界格局加速形成",
                    content = "2026年全球地缘政治格局持续深刻演变，多极化趋势日益明显。新兴经济体在国际事务中的话语权不断提升，多边合作机制发挥越来越重要的作用。\n\n分析人士指出，全球治理体系面临深刻变革，各国在气候变化、数字经济、国际安全等领域的合作与竞争并存。发展中国家群体性崛起成为不可逆转的历史潮流。\n\n和平与发展仍是时代主题，但地区热点问题依然存在，需要国际社会共同努力应对。",
                    summary = "多极化世界格局加速演变，新兴经济体影响力上升",
                    category = "军事",
                    author = "环球时报",
                    sourceUrl = "https://example.com/world-2026",
                    publishTime = System.currentTimeMillis() - 86400000
                ),
                NewsArticle(
                    id = 1008,
                    title = "年度票房冠军诞生：国产科幻电影刷新纪录",
                    content = "国产科幻大片《星际之门》上映一个月票房突破50亿，超越此前纪录成为国产电影票房冠军。影片以震撼的视觉效果和深刻的主题赢得了观众和评论家的一致好评。\n\n导演表示，影片历时五年打造，特效团队由国内外顶尖人才组成，展现了国内电影工业的快速进步。主演的精彩表演也为影片增色不少。\n\n该片的成功标志着国产科幻电影进入新纪元，预计将带动更多优质科幻作品的诞生。",
                    summary = "国产科幻电影票房突破50亿，创历史新高",
                    category = "娱乐",
                    author = "娱乐周刊",
                    sourceUrl = "https://example.com/movie-2026",
                    publishTime = System.currentTimeMillis() - 172800000
                )
            )
            for (news in demoNews) {
                repository.insertNews(news)
            }
        }
    }

    private fun loadChannels() {
        viewModelScope.launch {
            repository.fetchChannelsFromApi().collect { result ->
                when (result) {
                    is NetworkResult.Success -> {
                        val channels = result.data
                        _apiChannels.value = channels
                        if (channels.isNotEmpty()) {
                            _categories.value = channels.map {
                                Category(it.name, it.name, it.channelId.hashCode())
                            }
                            if (_currentChannelId.value == null) {
                                selectChannel(channels.first().channelId, channels.first().name)
                            }
                        }
                    }
                    is NetworkResult.Error -> {
                        _errorMessage.value = result.message
                        loadLocalNews()
                    }
                    is NetworkResult.Loading -> {}
                }
            }
        }
    }

    private fun loadLocalNews() {
        viewModelScope.launch {
            repository.getAllNews().collect { list ->
                _articles.value = list
            }
        }
    }

    fun selectChannel(channelId: String?, channelName: String) {
        _currentChannelId.value = channelId
        _currentCategory.value = channelName
        _isLoading.value = true

        if (channelId != null) {
            viewModelScope.launch {
                repository.fetchNewsFromApi(
                    channelId = channelId,
                    channelName = null
                ).collect { result ->
                    when (result) {
                        is NetworkResult.Success -> {
                            _articles.value = result.data
                            _isLoading.value = false
                        }
                        is NetworkResult.Error -> {
                            _errorMessage.value = result.message
                            _isLoading.value = false
                            loadLocalNewsByCategory(channelName)
                        }
                        is NetworkResult.Loading -> {}
                    }
                }
            }
        } else {
            loadLocalNewsByCategory(channelName)
            _isLoading.value = false
        }
    }

    private fun loadLocalNewsByCategory(category: String) {
        viewModelScope.launch {
            repository.getNewsByCategory(category).collect { list ->
                _articles.value = list
            }
        }
    }

    fun refreshFromApi() {
        viewModelScope.launch {
            _isRefreshing.value = true
            val channelId = _currentChannelId.value
            val channelName = _currentCategory.value
            repository.fetchNewsFromApi(
                channelId = channelId,
                channelName = if (channelId == null) channelName else null
            ).collect { result ->
                when (result) {
                    is NetworkResult.Success -> {
                        _articles.value = result.data
                        _isRefreshing.value = false
                    }
                    is NetworkResult.Error -> {
                        _errorMessage.value = result.message
                        _isRefreshing.value = false
                    }
                    is NetworkResult.Loading -> {}
                }
            }
        }
    }

    fun searchNews(keyword: String) {
        if (keyword.isBlank()) {
            refreshFromApi()
            return
        }
        _isLoading.value = true
        viewModelScope.launch {
            repository.fetchNewsFromApi(title = keyword).collect { result ->
                when (result) {
                    is NetworkResult.Success -> {
                        _articles.value = result.data
                        _isLoading.value = false
                    }
                    is NetworkResult.Error -> {
                        _errorMessage.value = result.message
                        _isLoading.value = false
                    }
                    is NetworkResult.Loading -> {}
                }
            }
        }
    }

    fun toggleFavorite(id: Long) {
        viewModelScope.launch {
            repository.toggleFavorite(id)
        }
    }

    fun incrementViewCount(id: Long) {
        viewModelScope.launch {
            repository.incrementViewCount(id)
        }
    }

    fun clearError() {
        _errorMessage.value = null
    }

    class Factory : ViewModelProvider.Factory {
        @Suppress("UNCHECKED_CAST")
        override fun <T : ViewModel> create(modelClass: Class<T>): T {
            return NewsViewModel() as T
        }
    }
}
