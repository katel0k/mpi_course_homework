const path = require('path');

module.exports = {
	entry: './index.js',
	mode: 'none',
	output: {
		path: path.resolve('./static/js'),
		filename: '[name].js',
	},
	resolve: {
	  	extensions: ['.js', '.jsx'],
	},
	module: {
		rules: [
			{
				test: /\.m?jsx?$/,
				exclude: /(node_modules|bower_components)/,
				use: {
					loader: "babel-loader"
				}
			}
		]
	}
}