axios.defaults.baseURL = "/api/";

const app = new Vue({
  el: "#app",
  data: {
    delay: 3,
    refreshInterval: null,
    dht: { temperature: null, humidity: null },
    noticeDistance: 18,
    distance: null,
    alert: false,
    alertTimeout: null,
  },
  computed: {
    distanceToClose: function () {
      return this.distance <= this.noticeDistance;
    },
  },
  watch: {
    distanceToClose: function (newDistanceToClose, oldDistanceToClose) {
      if (!oldDistanceToClose && newDistanceToClose) {
        this.$notify.warning({
          title: "警告",
          message: "可能有人靠近",
          duration: 0,
        });
      }
    },
    alert: function (newAlert, oldAlert) {
      if (!oldAlert && newAlert) {
        this.$notify.info({
          title: "提示",
          message: "您的门铃响了",
          duration: 2000,
        });
      }
    },
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
      this.postCommands(["DHT", "distance"]);
      this.getCommands();
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
      this.postCommands([command]);
    },
    postCommands: function (commands) {
      axios.post("/write", { commands: commands }).catch((err) => {
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
        } else if (command === "alert") {
          window.clearTimeout(this.alertTimeout);
          this.alert = true;
          this.alertTimeout = window.setTimeout(() => {
            this.alert = false;
          }, 2000);
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
    },
  },
});
