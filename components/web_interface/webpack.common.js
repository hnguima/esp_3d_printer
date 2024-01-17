const HtmlWebpackPlugin = require("html-webpack-plugin");
const CompressionPlugin = require("compression-webpack-plugin");
const path = require("path");

module.exports = {
  mode: "development",

  entry: "./src/index.js",

  plugins: [
    new HtmlWebpackPlugin({
      title: "Development",
    }),
    new CompressionPlugin({
      algorithm: "gzip",
      test: /.js$/,
      filename: "srv/[name].js.gz",
    }),
    new CompressionPlugin({
      algorithm: "gzip",
      test: /.html$/,
      filename: "srv/[name].html.gz",
    }),
  ],

  output: {
    filename: "[name].bundle.js",
    path: path.resolve(__dirname, "dist"),
  },

  module: {
    rules: [
      {
        test: /\.(js|jsx)$/,
        exclude: /node_modules/,
        use: { loader: "babel-loader" },
      },
      {
        test: /\.css$/,
        use: ["style-loader", "css-loader"],
      },
      {
        test: /\.(svg)$/i,
        type: "asset/inline",
      },
      {
        test: /\.(woff|woff2|eot|ttf|otf)$/i,
        type: "asset/inline",
      },
      {
        test: /\.json$/,
        type: "asset/resource",
      },
    ],
  },

  // devServer: {
  //   watchFiles: {
  //     paths: ["src/**/*.js", "dist/**/*"],
  //     options: {
  //       usePolling: true,
  //     },
  //   },
  // },

  watchOptions: {
    poll: 500,
    aggregateTimeout: 50,
    ignored: /node_modules/,
  },

  resolve: {
    alias: {
      Images: path.resolve(__dirname, "./src/img/"),
      App: path.resolve(__dirname, "./src/app/"),
      Fonts: path.resolve(__dirname, "./src/fonts/"),
      Components: path.resolve(__dirname, "./src/components/"),
      Tooltip: path.resolve(__dirname, "./src/components/tooltip/"),
      Page: path.resolve(__dirname, "./src/components/page/"),
      Sidebar: path.resolve(__dirname, "./src/components/sidebar/"),
      Field: path.resolve(__dirname, "./src/components/Field/"),
      Header: path.resolve(__dirname, "./src/components/header/"),
      Panel: path.resolve(__dirname, "./src/components/panel/"),
      Styles: path.resolve(__dirname, "./src/styles/"),
    },
  },
};
