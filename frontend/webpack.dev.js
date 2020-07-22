const path = require('path');
const HtmlWebpackPlugin = require("html-webpack-plugin");

module.exports = {
  mode: "development",
  entry: './src/index.js',
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: 'bundle.js'
  },
  devServer: {
    historyApiFallback: true,
    stats: "minimal"
  },
  devtool: "cheap-module-eval-source-map",
  plugins: [new HtmlWebpackPlugin()]
};
