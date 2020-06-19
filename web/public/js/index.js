axios.defaults.baseURL = "/api/";

const app = new Vue({
  el: "#app",
  data: {
    delay: 30,
    refreshInterval: null,
    dht: { temperature: null, humidity: null },
    noticeDistance: 18,
    distance: null,
  },
  created: function () {
    this.refresh();
    this.setRefreshInterval();
  },
  beforeDestroy: function () {
    window.clearInterval(this.refreshInterval);
  },
  methods: {
    setRefreshInterval: function () {
      window.clearInterval(this.refreshInterval);
      this.refreshInterval = window.setInterval(
        this.refresh,
        this.delay * 1000
      );
    },
    refresh: function () {
      this.postCommand("DHT");
      this.postCommand("distance");
      this.getCommands();
    },
    postDht: function () {
      this.postCommand("DHT");
    },
    warning: function () {
      this.postCommand("warning");
    },
    getCommands: function () {
      axios
        .get("/read")
        .then((res) => {
          this.execCommands(res.data.commands);
        })
        .catch((err) => {
          console.log(err);
        });
    },
    postCommand: function (command) {
      axios.post("/write", { command: command }).catch((err) => {
        console.log(err);
      });
    },
    execCommands: function (commands) {
      commands.forEach((value) => {
        const data = value.split(" ");
        const command = data.shift();
        if (command === "DHT") {
          this.parseDht(data);
        } else if (command === "distance") {
          this.parseDistance(data);
        }
      });
    },
    parseDht: function (data) {
      this.dht.temperature = data[0] + " " + data[1];
      this.dht.humidity = data[2] + " " + data[3];
    },
    parseDistance: function (data) {
      const distance = parseFloat(data[0]);
      this.distance = distance;
      if (distance < this.noticeDistance) {
        this.$notify.warning({
          title: "警告",
          message: "可能有人靠近",
        });
      }
    },
  },
});
