var colors = ["#727cf5", "#0acf97", "#fa5c7c", "#ffbc00"];
var data_colors = $("#temperature-chart").data("colors");
data_colors && (colors = data_colors.split(","));

var temperature_options = {
    chart: { type: "line", height: 60, sparkline: { enabled: !0 } },
    series: [{ data: [25, 66, 41, 89, 63, 25, 44, 12, 36, 9, 54] }],
    stroke: { width: 2, curve: "smooth" },
    markers: { size: 0 },
    colors: colors,
    tooltip: {
        fixed: { enabled: !1 },
        x: { show: !1 },
        y: {
            title: {
                formatter: function (o) {
                    return "";
                },
            },
        },
        marker: { show: !1 },
    },
};

var temperature_chart = new ApexCharts(document.querySelector("#temperature-chart"), temperature_options);
temperature_chart.render();

(data_colors = $("#dew-point-chart").data("colors")) && (colors = data_colors.split(","));

var dew_point_options = {
    chart: { type: "line", height: 60, sparkline: { enabled: !0 } },
    series: [{ data: [25, 66, 41, 89, 63, 25, 44, 12, 36, 9, 54] }],
    stroke: { width: 2, curve: "smooth" },
    markers: { size: 0 },
    colors: colors,
    tooltip: {
        fixed: { enabled: !1 },
        x: { show: !1 },
        y: {
            title: {
                formatter: function (o) {
                    return "";
                },
            },
        },
        marker: { show: !1 },
    },
};

var dew_point_chart = new ApexCharts(document.querySelector("#dew-point-chart"), dew_point_options);
dew_point_chart.render();

(data_colors = $("#rain-level-chart").data("colors")) && (colors = data_colors.split(","));

var rain_level_options = {
    chart: { type: "line", height: 60, sparkline: { enabled: !0 } },
    series: [{ data: [25, 66, 41, 89, 63, 25, 44, 12, 36, 9, 54] }],
    stroke: { width: 2, curve: "smooth" },
    markers: { size: 0 },
    colors: colors,
    tooltip: {
        fixed: { enabled: !1 },
        x: { show: !1 },
        y: {
            title: {
                formatter: function (o) {
                    return "";
                },
            },
        },
        marker: { show: !1 },
    },
};

var rain_level_chart = new ApexCharts(document.querySelector("#rain-level-chart"), rain_level_options);
rain_level_chart.render();

(data_colors = $("#luminosity-chart").data("colors")) && (colors = data_colors.split(","));

var luminosity_options = {
    chart: { type: "line", height: 60, sparkline: { enabled: !0 } },
    series: [{ data: [25, 66, 41, 89, 63, 25, 44, 12, 36, 9, 54] }],
    stroke: { width: 2, curve: "smooth" },
    markers: { size: 0 },
    colors: colors,
    tooltip: {
        fixed: { enabled: !1 },
        x: { show: !1 },
        y: {
            title: {
                formatter: function (o) {
                    return "";
                },
            },
        },
        marker: { show: !1 },
    },
};

var luminosity_chart = new ApexCharts(document.querySelector("#luminosity-chart"), luminosity_options);
luminosity_chart.render();

var humidity_chart;

!(function (o) {
    "use strict";

    function e() {
        (this.$body = o("body")), (this.charts = []);
    }

    (e.prototype.initCharts = function () {
        window.Apex = { chart: { parentHeightOffset: 0, toolbar: { show: !1 } }, grid: { padding: { left: 0, right: 0 } }, colors: ["#727cf5", "#0acf97", "#fa5c7c", "#ffbc00"] };
        var e = ["#727cf5", "#0acf97", "#fa5c7c", "#ffbc00"],
            t = o("#humidity-chart").data("colors");
        t && (e = t.split(","));
        var humidity_options = {
            chart: { height: 320, type: "line", dropShadow: { enabled: !0, opacity: 0.2, blur: 7, left: -7, top: 7 } },
            dataLabels: { enabled: 0 },
            stroke: { curve: "smooth", width: 4 },
            series: [
                { name: "Umidade do ar", data: [10, 20, 15, 25, 20, 30] }, 
                { name: "Umidade do solo", data: [0, 15, 10, 30, 15, 35] },
            ],
            colors: e,
            zoom: { enabled: !1 },
            legend: { show: !1 },
            xaxis: { 
                labels: {
                    show: 0
                },
            },
            yaxis: {
                labels: {
                    formatter: function (e) {
                        return e + "%";
                    },
                    offsetX: -15,
                },
            },
        };
        humidity_chart = new ApexCharts(document.querySelector("#humidity-chart"), humidity_options);
        humidity_chart.render();
    }),
        (e.prototype.init = function () {
            this.initCharts();
        }),
        (o.Dashboard = new e()),
        (o.Dashboard.Constructor = e);
})(window.jQuery),
    (function (t) {
        "use strict";
        t(document).ready(function (e) {
            t.Dashboard.init();
        });
    })(window.jQuery);

(data_colors = $("#water-reservoir-chart").data("colors")) && (colors = data_colors.split(","));

var water_reservoir_options = {
    chart: { height: 340, type: "radialBar" },
    plotOptions: {
        radialBar: {
            hollow: { size: "60%" },
            startAngle: -135,
            endAngle: 135,
            dataLabels: {
                name: { fontSize: "16px", color: void 0, offsetY: 86 },
                value: {
                    offsetY: 0,
                    fontSize: "22px",
                    color: void 0,
                    formatter: function (o) {
                        return o + "%";
                    },
                },
            },
        },
    },
    fill: { gradient: { enabled: !0, shade: "dark", shadeIntensity: 0.15, inverseColors: !1, opacityFrom: 1, opacityTo: 1, stops: [0, 50, 65, 91] } },
    stroke: { dashArray: 0 },
    colors: colors,
    series: [89.00],
    labels: [["890.00 L", "1000.00 L"]],
    responsive: [{ breakpoint: 340, options: { chart: { height: 280 } } }],
};

var water_reservoir_chart = new ApexCharts(document.querySelector("#water-reservoir-chart"), water_reservoir_options);
water_reservoir_chart.render();

(data_colors = $("#water-consumption-chart").data("colors")) && (colors = data_colors.split(","));

var water_consumption_options = {
    chart: {
        height: 340,
        type: "bar",
        toolbar: { show: !1 },
        events: {
            click: function (o, a, t) {
                console.log(o, a, t);
            },
        },
    },
    colors: colors,
    plotOptions: { bar: { columnWidth: "50%", distributed: !1 } },
    dataLabels: { enabled: !1 },
    series: [{ name: "Consumo", data: [20, 42, 68, 55, 80, 25, 40, 0, 0, 0, 0, 0] }],
    xaxis: { categories: ["Jan.", "Fev.", "Mar.", "Abr.", "Maio", "Jun.", "Jul.", "Ago.", "Set.", "Out.", "Nov.", "Dez."], labels: { style: { colors: colors, fontSize: "14px" } } },
    yaxis: {
        labels: {
            formatter: function (o) {
                return o.toFixed(2) + " L";
            },
        },
    },
    legend: { offsetY: 7 },
    grid: { row: { colors: ["transparent", "transparent"], opacity: 0.2 }, borderColor: "#f1f3fa" },
};

var water_consumption_chart = new ApexCharts(document.querySelector("#water-consumption-chart"), water_consumption_options);
water_consumption_chart.render();

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function celsius_to_kelvin(temperature) {
    return temperature + 273.15;
}

function celsius_to_fahrenheit(temperature) {
    return (temperature * 1.8) + 32;
}

function full_values_list(data) {
    if (data.length < 60) {
        for (var i = data.length; i < 60; i++) {
            data.unshift(0);
        }
    }

    return data;
}

function full_values_list_inversed(data) {
    if (data.length < 60) {
        for (var i = data.length; i < 60; i++) {
            data.unshift(0);
        }
    }

    return data;
}

async function update_data() {
    while (true) {
        var sensors_now = JSON.parse(request_get("/api/sensors/?filter=latest"))[0];
        var settings = JSON.parse(request_get("/api/settings/"))[0];

        var temperature, dew_point;

        switch (settings.fields.temperature_measurement) {
            case "kelvin":
                temperature = celsius_to_kelvin(sensors_now.fields.temperature).toFixed(2) + " K";
                dew_point = celsius_to_kelvin(sensors_now.fields.dew_point).toFixed(2) + " K";
                break;

            case "fahrenheit":
                temperature = celsius_to_fahrenheit(sensors_now.fields.temperature).toFixed(2) + " °F";
                dew_point = celsius_to_fahrenheit(sensors_now.fields.dew_point).toFixed(2) + " °F";
                break;

            default:
                temperature = (sensors_now.fields.temperature).toFixed(2) + " °C";
                dew_point = (sensors_now.fields.dew_point).toFixed(2) + " °C";
                break;
        }

        $("#temperature").text(temperature);
        $("#dew-point").text(dew_point);

        $("#rain-level").text((sensors_now.fields.rain_level).toExponential(2) + "%");
        $("#luminosity").text((sensors_now.fields.luminosity).toExponential(2) + " lx");

        var data_stack = [];
        var latest_sensor_data = JSON.parse(request_get("/api/sensors/?filter=latest&limit=60")).reverse();

        for (var data of latest_sensor_data) {
            switch (settings.fields.temperature_measurement) {
                case "kelvin":
                    data_stack.push(celsius_to_kelvin(data.fields.temperature).toFixed(2));
                    break;

                case "fahrenheit":
                    data_stack.push(celsius_to_fahrenheit(data.fields.temperature).toFixed(2));
                    break;

                default:
                    data_stack.push((data.fields.temperature).toFixed(2));
                    break;
            }
        }

        temperature_chart.updateSeries([{
            data: full_values_list_inversed(data_stack)
        }]);

        data_stack = [];

        for (var data of latest_sensor_data) {
            switch (settings.fields.temperature_measurement) {
                case "kelvin":
                    data_stack.push(celsius_to_kelvin(data.fields.dew_point).toFixed(2));
                    break;

                case "fahrenheit":
                    data_stack.push(celsius_to_fahrenheit(data.fields.dew_point).toFixed(2));
                    break;

                default:
                    data_stack.push((data.fields.dew_point).toFixed(2));
                    break;
            }
        }

        dew_point_chart.updateSeries([{
            data: full_values_list_inversed(data_stack)
        }]);

        data_stack = [];

        for (var data of latest_sensor_data) {
            data_stack.push((data.fields.rain_level).toFixed(2));
        }

        rain_level_chart.updateSeries([{
            data: full_values_list_inversed(data_stack)
        }]);

        data_stack = [];

        for (var data of latest_sensor_data) {
            data_stack.push(data.fields.luminosity);
        }

        luminosity_chart.updateSeries([{
            data: full_values_list_inversed(data_stack)
        }]);

        $("#humidity").text(sensors_now.fields.humidity + "%");
        $("#soil-moisture").text(sensors_now.fields.soil_moisture + "%");

        var humidity = [], soil_moisture = [];

        for (var data of latest_sensor_data) {
            humidity.push(data.fields.humidity);
            soil_moisture.push(data.fields.soil_moisture);
        }

        humidity_chart.updateSeries([
            { data: full_values_list(humidity) },
            { data: full_values_list(soil_moisture) }
        ]);

        $("#luminosity").text((sensors_now.fields.luminosity).toFixed(2) + " lx");
        $("#rain-level").text(sensors_now.fields.rain_level + "%");

        var percentage = 0;
        var is_empty = false;
        var sensors_last_hour = JSON.parse(request_get("/api/sensors/?filter=last_hour"));

        if (sensors_last_hour.length == 0) {
            is_empty = true;

            sensors_last_hour = {
                fields: {
                    temperature: 0,
                    dew_point: 0,
                    rain_level: 0,
                    luminosity: 0
                }
            }
        } else {
            is_empty = false;
            sensors_last_hour = sensors_last_hour[0];
        }

        if (sensors_now.fields.temperature >= sensors_last_hour.fields.temperature) {
            percentage = ((sensors_now.fields.temperature - sensors_last_hour.fields.temperature) / sensors_last_hour.fields.temperature) * 100;
            $("#temperature-arrow").removeClass("text-danger mdi mdi-arrow-down-bold");
            $("#temperature-arrow").addClass("text-success mdi mdi-arrow-up-bold");
            $("#temperature-last-hour").removeClass("text-danger");
            $("#temperature-last-hour").addClass("text-success");
        } else {
            percentage = ((sensors_last_hour.fields.temperature - sensors_now.fields.temperature) / sensors_now.fields.temperature) * 100;
            $("#temperature-arrow").removeClass("text-success mdi mdi-arrow-up-bold");
            $("#temperature-arrow").addClass("text-danger mdi mdi-arrow-down-bold");
            $("#temperature-last-hour").removeClass("text-success");
            $("#temperature-last-hour").addClass("text-danger");
        }

        if (is_empty) {
            percentage = 0.0;
        }

        $("#temperature-last-hour").text(percentage.toFixed(2) + "%");

        if (sensors_now.fields.dew_point >= sensors_last_hour.fields.dew_point) {
            percentage = ((sensors_now.fields.dew_point - sensors_last_hour.fields.dew_point) / sensors_last_hour.fields.dew_point) * 100;
            $("#dew-point-arrow").removeClass("text-danger mdi mdi-arrow-down-bold");
            $("#dew-point-arrow").addClass("text-success mdi mdi-arrow-up-bold");
            $("#dew-point-last-hour").removeClass("text-danger");
            $("#dew-point-last-hour").addClass("text-success");
        } else {
            percentage = ((sensors_last_hour.fields.dew_point - sensors_now.fields.dew_point) / sensors_now.fields.dew_point) * 100;
            $("#dew-point-arrow").removeClass("text-success mdi mdi-arrow-up-bold");
            $("#dew-point-arrow").addClass("text-danger mdi mdi-arrow-down-bold");
            $("#dew-point-last-hour").removeClass("text-success");
            $("#dew-point-last-hour").addClass("text-danger");
        }

        if (is_empty) {
            percentage = 0.0;
        }

        $("#dew-point-last-hour").text(percentage.toFixed(2) + "%");

        if (sensors_now.fields.rain_level >= sensors_last_hour.fields.rain_level) {
            percentage = ((sensors_now.fields.rain_level - sensors_last_hour.fields.rain_level) / sensors_last_hour.fields.rain_level) * 100;
            $("#rain-level-arrow").removeClass("text-danger mdi mdi-arrow-down-bold");
            $("#rain-level-arrow").addClass("text-success mdi mdi-arrow-up-bold");
            $("#rain-level-last-hour").removeClass("text-danger");
            $("#rain-level-last-hour").addClass("text-success");
        } else {
            percentage = ((sensors_last_hour.fields.rain_level - sensors_now.fields.rain_level) / sensors_now.fields.rain_level) * 100;
            $("#rain-level-arrow").removeClass("text-success mdi mdi-arrow-up-bold");
            $("#rain-level-arrow").addClass("text-danger mdi mdi-arrow-down-bold");
            $("#rain-level-last-hour").removeClass("text-success");
            $("#rain-level-last-hour").addClass("text-danger");
        }

        if (is_empty) {
            percentage = 0.0;
        }

        $("#rain-level-last-hour").text(percentage.toFixed(2) + "%");

        if (sensors_now.fields.luminosity >= sensors_last_hour.fields.luminosity) {
            percentage = ((sensors_now.fields.luminosity - sensors_last_hour.fields.luminosity) / sensors_last_hour.fields.luminosity) * 100;
            $("#luminosity-arrow").removeClass("text-danger mdi mdi-arrow-down-bold");
            $("#luminosity-arrow").addClass("text-success mdi mdi-arrow-up-bold");
            $("#luminosity-last-hour").removeClass("text-danger");
            $("#luminosity-last-hour").addClass("text-success");
        } else {
            percentage = ((sensors_last_hour.fields.luminosity - sensors_now.fields.luminosity) / sensors_now.fields.luminosity) * 100;
            $("#luminosity-arrow").removeClass("text-success mdi mdi-arrow-up-bold");
            $("#luminosity-arrow").addClass("text-danger mdi mdi-arrow-down-bold");
            $("#luminosity-last-hour").removeClass("text-success");
            $("#luminosity-last-hour").addClass("text-danger");
        }

        if (is_empty) {
            percentage = 0.0;
        }

        $("#luminosity-last-hour").text(percentage.toFixed(2) + "%");

        water_reservoir_chart.updateOptions({
            series: [(sensors_now.fields.percentage_water).toFixed(2)],
            labels: [[(sensors_now.fields.amount_water).toFixed(2) + " L", (settings.fields.reservoir_capacity).toFixed(2) + " L"]]
        });

        var months = [];
        var current_year = new Date().getFullYear();

        var water_consumption = JSON.parse(request_get(`/api/water-consumption/?filter=consumption_sum&year=${current_year}`));

        for (var key of Object.keys(water_consumption.month)) {
            months.push((water_consumption.month[key]).toFixed(2));
        }

        water_consumption_chart.updateSeries([{
            data: months
        }]);

        await sleep(4000);
    }
}

$(document).ready(function() {
    update_data();
});