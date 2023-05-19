var webpack = require("webpack");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const CompressionPlugin = require("compression-webpack-plugin");
const ImageMinimizerPlugin = require("image-minimizer-webpack-plugin");

const path = require("path");
var fs = require("fs");

var dirents = fs.readdirSync("./src/app/", { withFileTypes: true });
const pages = dirents
  .filter(
    (dirent) =>
      dirent.isFile() &&
      !dirent.name.includes(".json") &&
      !dirent.name.includes(".pb")
  )
  .map((dirent) => dirent.name.split(".")[0]);

console.log(pages);

module.exports = {
  entry: pages.reduce((config, page) => {
    config[page] = `./src/app/${page}.js`;
    return config;
  }, {}),

  output: {
    filename: "[name].js", // bundle
    path: path.resolve(__dirname, "dist"),
    clean: true,
    assetModuleFilename: "[name][ext]",
  },

  module: {
    rules: [
      {
        test: /\.css$/,
        use: ["css-loader"],
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

  optimization: {
    minimize: true,
    minimizer: [
      "...",
      new ImageMinimizerPlugin({
        minimizer: {
          implementation: ImageMinimizerPlugin.svgoMinify,
          options: {
            encodeOptions: {
              // Pass over SVGs multiple times to ensure all optimizations are applied. False by default
              multipass: true,
              plugins: [
                // set of built-in plugins enabled by default
                // see: https://github.com/svg/svgo#default-preset
                "preset-default",
              ],
            },
          },
        },
      }),
    ],
    splitChunks: {
      chunks: "all",
    },
  },

  plugins: [].concat(
    pages.map(
      (page) =>
        new HtmlWebpackPlugin({
          inject: true,
          // template: `./${page}.html`,
          filename: `${page}.html`,
          chunks: [page],
        })
    ),
    new CompressionPlugin({
      algorithm: "gzip",
      test: /.js$/,
      filename: "srv/[name].js.gz",
    }),
    new CompressionPlugin({
      algorithm: "gzip",
      test: /.html$/,
      filename: "srv/[name].html.gz",
    })
  ),

  resolve: {
    alias: {
      Images: path.resolve(__dirname, "./src/img/"),
      App: path.resolve(__dirname, "./src/app/"),
      Fonts: path.resolve(__dirname, "./src/fonts/"),
      Base: path.resolve(__dirname, "./src/base/"),
      Tooltip: path.resolve(__dirname, "./src/base/tooltip/"),
      Page: path.resolve(__dirname, "./src/base/page/"),
      Sidebar: path.resolve(__dirname, "./src/base/sidebar/"),
      Field: path.resolve(__dirname, "./src/base/field/"),
      Header: path.resolve(__dirname, "./src/base/header/"),
      Panel: path.resolve(__dirname, "./src/base/panel/"),
      Styles: path.resolve(__dirname, "./src/styles/"),
    },
  },
};
