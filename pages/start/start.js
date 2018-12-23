Page({

  /**
   * 页面的初始数据
   */
  data: {

    music_list: [
      {
        name: "BUPT Theme Song",
        length: 200
      },
      {
        name: "Fade",
        length: 200
      },
      {
        name: "Faded",
        length: 200
      },
      {
        name: "Flavor Dance",
        length: 200
      },
      {
        name: "Always with Me",
        length: 257
      },
      {
        name: "Plants vs Zombies",
        length: 200
      },
      {
        name: "Kiss the Rain",
        length: 200
      },
      {
        name: "Sundial Dreams",
        length: 200
      },
      {
        name: "The First Snowflakes",
        length: 200
      }],
    play: "https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1512218011639&di=47d229f8166e09fbcb360285db64db8a&imgtype=0&src=http%3A%2F%2Fpic.58pic.com%2F58pic%2F14%2F64%2F28%2F05T58PICd5S_1024.jpg",
    next: "https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1512210880414&di=ae93550e8f8c5e0169361061673f5e24&imgtype=0&src=http%3A%2F%2Fimgsrc.baidu.com%2Fimgad%2Fpic%2Fitem%2F0e2442a7d933c895ac5bcf50da1373f0820200f7.jpg",
    previous: "https://ss1.bdstatic.com/70cFuXSh_Q1YnxGkpoWK1HF6hhy/it/u=3626158946,5519961&fm=27&gp=0.jpg",
    playId: 0,
    //0:pause,1:play
    playButtonStatus: 0,
    playPrecent: 0,
    canPlay: false,
    vol: 20
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {

  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {
    // wx.showToast({
    //   title: 'onShow',
    //   duration:500
    // })
    this.autoPlus()
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  },
  //Function of the function is to auto move slide  ,every second ++
  
  
  autoPlus: function () {
    if (this.data.playButtonStatus == 1 && this.data.canPlay) {
      if (this.data.playPrecent < this.data.music_list[this.data.playId].length) {
        var interval = 100 / this.data.music_list[this.data.playId].length;
        this.setData({
          playPrecent: this.data.playPrecent + interval
        })
      }
      else {
        this.setData({
          playPrecent: 0
        })
        this.NextMusic()
      }
    }
    setTimeout(this.autoPlus, 1000)
  },
  
  
  sendRequest: function (obj) {
    wx.request(obj);
  },
  
  
  makeObj: function (i, sta, pre, msg) {
    var obj = {
      url: "https://api.heclouds.com/devices/503152785/datapoints?type=3",
      header: {
        "api-key": "kPtZsRKG5Mh2YZZnK29G4r=FVQg=",
        "Content-Type": "application/json"
      },
      method: "post",
      data: {
        //msuci id,playing status,playing precent
        "id": i,
        "status": sta,
        "vol": this.data.vol
        //"precent":pre
      },
      success: function (res) {
        if (msg != "") {
          wx.showToast({
            title: msg,
            duration: 1000
          })
          //console.log(i);
        }
      }
    }
    return obj;
  },
  // timeChange:function(){
  //   console.log("dododo")
  //   // for(;this.data.playPrecent<this.data.music_list[this.data.playId].length;){
  //   //   setTimeout(function(){
  //   //     if(this.data.playButtonStatus=="playing"){
  //       if (this.data.playPrecent >= this.data.music_list[this.data.playId].length) {
  //         this.setData({
  //           playPrecent: 0

  //         })
  //         this.nextMusic()
  //       } else {
  //         this.setData({
  //           playPrecent: this.playPrecent + 1
  //         })
  //       }
  //     //   }
  //     // },1000)
  //   // }
  // },
  
  
  chooseMusic: function (event) {
    var temp = 0;
    //Get Choosed Music id
    var musicName = event.currentTarget.dataset.id
    for (var i = 0; i < this.data.music_list.length; i++) {
      if (musicName == this.data.music_list[i].name) {
        temp = i;
        this.sendRequest(this.makeObj(i, 1, 0, "成功播放"))
        break
      }
    }
    this.setData({
      playPrecent: 0,
      playId: temp,
      canPlay: true,
      playingMusic: this.data.music_list[this.data.playId],
      play: "https://ss0.bdstatic.com/94oJfD_bAAcT8t7mm9GUKT-xh_/timg?image&quality=100&size=b4000_4000&sec=1512287432&di=0d1c4453ce9a687ec14264e9e24e55bf&src=http://pic.58pic.com/58pic/14/80/56/85Y58PICdcV_1024.jpg",
      playButtonStatus: 1
    })
    var sta = this.data.playButtonStatus
    this.sendRequest(this.makeObj(this.data.playId, sta, 0, ""))
    // this.ProgressPlus()
  },
  
  
  PreviousMusic: function () {
    var currentId = this.data.playId;
    var finalId = 0;
    if (currentId >= 1) {
      finalId = currentId - 1
    }
    else {
      finalId = this.data.music_list.length - 1
    }
    if (this.data.playId!=0) var musicName = this.data.music_list[this.data.playId-1].name
    else var musicName = this.data.music_list[8].name
    this.sendRequest(this.makeObj(this.data.playId-1, 1, 0, "成功后退"))
    this.setData({
      playId: finalId,
      playPrecent: 0,
      playingMusic: this.data.music_list[this.data.playId]
    })
  },
  
  
  NextMusic: function () {
    var currentId = this.data.playId;
    var finalId = 0;
    if (currentId < this.data.music_list.length-1) {
      finalId = currentId + 1;
    }
    else {
      finalId = 0;
    }
    if (this.data.playId!=8) var musicName = this.data.music_list[this.data.playId+1].name
    else var musicName = this.data.music_list[1].name
    this.sendRequest(this.makeObj(this.data.playId+1, 1, 0, "成功跳过"))
    this.setData({
      playId: finalId,
      playPrecent: 0,
      playingMusic: this.data.music_list[this.data.playId]
    })
    //this.ProgressPlus()
    //console.log(this.data.playId);
  },
  
  
  PlayOrPause: function () {
    var pre = this.data.playPrecent
    if (this.data.playButtonStatus == 0) {
      this.data.playButtonStatus = 1;
      if (!this.data.canPlay) {
        this.setData({
          canPlay: true
        })
      }
      this.setData({
        play: "https://ss0.bdstatic.com/94oJfD_bAAcT8t7mm9GUKT-xh_/timg?image&quality=100&size=b4000_4000&sec=1512287432&di=0d1c4453ce9a687ec14264e9e24e55bf&src=http://pic.58pic.com/58pic/14/80/56/85Y58PICdcV_1024.jpg"
      })
    }
    else {
      this.data.playButtonStatus = 0;
      this.setData({
        play: "https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1512218011639&di=47d229f8166e09fbcb360285db64db8a&imgtype=0&src=http%3A%2F%2Fpic.58pic.com%2F58pic%2F14%2F64%2F28%2F05T58PICd5S_1024.jpg"
      })

    }
    var sta = this.data.playButtonStatus
    //this.ProgressPlus()
    this.sendRequest(this.makeObj(this.data.playId, sta, pre, ""))

  },
  //I discovey that mcookie doesn't support paly at pointed time,so ,,,,the mothod is wroten but no where to use
  
  
  dragedProgress: function (e) {
    /*
    this.data.playPrecent = e.detail.value
    var sta=this.data.playButtonStatus
    var pre=this.data.playPrecent
    //this.ProgressPlus()
    this.sendRequest(this.makeObj(this.data.playId, sta, pre, ""))
    
    */
  },
  
  
  vol_change: function (e) {
    this.setData({
      vol: e.detail.value
    });
    var sta = this.data.playButtonStatus;
    var pre = this.data.playPrecent;
    this.sendRequest(this.makeObj(this.data.playId, sta, pre, ""));

  }
})