require 'CSV'
require 'cgi'
require "lemmatizer"
require 'httpclient'
require 'json'
require 'erb'
require 'colorize'
require 'thor'

# constants
PREVIEW_FILE_NAME = "preview.html"
SKUS_INPUT_FILE_NAME = ARGV[0]
BOW_FILE_NAME = "bag-of-words.txt"
CLUSTERER_INPUT_FILE_NAME = "sim.txt"
CLUSTERER_OUPUT_FILE_NAME = "cluster-index.txt"
SEGMENT = ARGV[1] || "women"

class Cluster
	attr_accessor :products
    attr_accessor :keys
end

class DebugInfo
	attr_accessor :jaccardIndex
    attr_accessor :invertedJaccardIndex
    attr_accessor :transformedJaccard1Index
    attr_accessor :transformedJaccardIndex
end

def jaccardIndex(text1, text2)
	union = text1 | text2
	intersect = text1 & text2
	intersect.count.to_f / union.count.to_f
end

def jaccardTransform1(index)
	(-Math::log(index,2))+1
end

def jaccardTransform(index)
	#((0.25-index)*10)**10
	if index == 0
		-999999999999999
	else
		(-jaccardTransform1(index)**10) +1
	end
end

def parse_data(inputFilePath)
	file = File.new(inputFilePath, "r")
	rawData = file.read
	file.close

	# parse data
	CSV::Converters[:blank_to_nil] = lambda do |field|
  		field && field.empty? ? nil : field
	end
	csv = CSV.new(rawData, :headers => true, :header_converters => :symbol, :converters => [:all, :blank_to_nil])
	productArray = csv.to_a.map {|row| row.to_hash }

	# url encode brand and lemmatize name
	lem = Lemmatizer.new
	productArray.map do |product| 
		brand = [CGI.escape(product[:brand].downcase)]
		product[:bow] = product[:color].downcase.scan(/\w+/)
		productNameTokens = product[:name].downcase.scan(/\w+/)
		productNameTokens.map do |string|
			product[:bow] += [lem.lemma(string)]
		end
		product[:bow].uniq!
	end
	return productArray
end

def clean_data(productArray)
	# remove words that only occur once
	# as they make the data set too sparse
	counts = Hash.new 0
	productArray.each do |product|
		product[:bow].each do |string|
			counts[string] += 1
		end
	end

	singles = Array.new()
	counts.each do |string, count|
		if count == 1
			singles.push(string)
		end
	end

	productArray.each do |product|
		product[:bow] = product[:bow] - singles
	end
	file = File.new(BOW_FILE_NAME,"w")
	# output processed data
	productArray.map do |product| 
		file << product[:sku] + ": #{product[:bow]} \n"
	end
	file.close

	# remove if no bag of words
	productArray.reject! { |product| product[:bow].count == 0 }
	return productArray
end

def calculate_similarities(outfile, productArray)
	# calculate Jaccard Index for similarities
	# map to [-inf,0] 
	# output with indexes to file
	file = File.new(outfile,"w")
	n = productArray.count - 1
	numOfComparisons = ((n*n) + n)/2
	file << "#{numOfComparisons} #{productArray.count}\n"
	debug = Array.new()
	productArray.each do |product1|
		productArray.each do |product2|
			i = productArray.index(product1)
			j = productArray.index(product2)
			if j > i
				index = jaccardIndex(product1[:bow],product2[:bow])
				info = DebugInfo.new
				info.jaccardIndex = index
				info.invertedJaccardIndex = 1 - index
				info.transformedJaccard1Index = jaccardTransform1(index)
				info.transformedJaccardIndex = jaccardTransform(index)
				debug.push(info)
				file << "#{i} #{j} #{jaccardTransform(index)}\n"
			end
		end
	end
	file.close

	debug.sort! { |info1, info2| info1.jaccardIndex <=> info2.jaccardIndex }
	debug.each do |info| 
		#print "#{info.jaccardIndex} ".red
		#print "#{info.invertedJaccardIndex} ".blue
		#print "#{info.transformedJaccard1Index} ".yellow
		#print "#{info.transformedJaccardIndex}\n".green
	end
end

def cluster(similarities,productArray)
	puts "---------------------------------------------------------".blue
	puts "--------------------clustering...------------------------".blue
	puts "---------------------------------------------------------".blue

	# cluster
	system("./ap2 #{similarities} #{CLUSTERER_OUPUT_FILE_NAME}")
	file = File.new(CLUSTERER_OUPUT_FILE_NAME, "r")
	clusters = Hash.new
	count = 0
	file.each_line do |line|
		if !clusters[line.to_i]
			clusters[line.to_i] = [productArray[count]]
		else
			clusters[line.to_i].push(productArray[count])
		end
		count = count + 1
	end
	file.close

	# sort the clusters

	orderedClusters = Array.new
	clusters.each do |index,products|
		cluster = Cluster.new
		cluster.products = products
		# find the keys of each cluster
		allKeyWords = Array.new
		products.each { |product| allKeyWords.concat(product[:bow]) }
		keyWordDictionary = Hash.new(0)
		allKeyWords.each do |keyword|
			keyWordDictionary[keyword] += 1
		end
		# remove all words that occur only once
		# arbitrarily decide the the keys must at least occur half the time
		keyWordDictionary.select! { |keyword, count| count > cluster.products.count/2}
		cluster.products = products
		cluster.keys = keyWordDictionary.keys
		orderedClusters.push(cluster)
	end

	orderedClusters.sort! { |clusterA,clusterB| clusterB.products.count <=> clusterA.products.count }

	queries = Array.new
	# output the clusters
	count = 1
	orderedClusters.each do |cluster|
		# decide which clusters to use
		# arbitrarily decide that cluster size has to be larger than 2
		if cluster.products.count > 2 && cluster.keys.count > 0
			puts "===========Cluster #{count}================".green
			queries.push(CGI.unescape(cluster.keys.join(" ")))
		else
			puts "===========Cluster #{count}(rejected)======".magenta
		end
		puts "Keys : #{cluster.keys}"
		cluster.products.each do |product|
			puts "#{product[:sku]} #{product[:bow]}"
		end
		count = count+1
	end

	return queries.uniq
end

def get_products(queries,productArray)
	if queries.count == 0
		puts "sorry i don't know what to reccommend".red
		exit()	
	end

	puts "---------------------------------------------------------".blue
	puts "--------------getting products from api------------------".blue
	puts "---------------------------------------------------------".blue

	# make queries for products
	http = HTTPClient.new
	url = "https://api.zalora.sg/v1/products"
	results = Array.new
	queries.each do |query|
		response = http.get(url,{"query"=>query,"segment"=>SEGMENT},{ 'Accept' => 'application/json' })
		if response.status == 200
			productList = JSON.parse(response.body)
			array = productList["data"]["products"]
			array.delete_at(0)
			array.delete_at(0)
			if results.count == 0
		 		results = array
			else
			 	results = results.zip(array).flatten.compact
			end 
			puts "got #{productList["data"]["product_count"]} products for " + query
		else
			puts "get products for #{query} failed"
		end
	end

	# remove duplicate products
	results.uniq!
	# remove products already viewed
	skusViewed = productArray.map { |product| product[:sku] }
	results.select! do |product|
		!skusViewed.include?product["config_sku"]
	end
	return results
end

def template
		%{
	        <DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
	
	    <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	    <head>
	            <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
	            <title>Reccommended products</title>
				<style>
	 			#container div {
	 			 display: inline-block;
	 			 margin: 0 1em 0 1em;
	 			 width: 15%;
				}
				</style>
	        </head>
	        <body>
	        <div id="container">
	               <% for product in results %>
	               <div>
	                  <img src="<%= product["main_image_url"] %>" width="150" height="200"></img>
	                  <p><b><%= product["brand"] %></b></p>
	                  <p><%= product["name"] %></p>
	               </div>
	               <% end %>
	        </div>
	        </body>
	        </html>
	  }
	end

class Cli < Thor
	desc(
    'reccomend <input filepath> <segment>(default to women)','tries to reccomend products based on user product view history'
    )
    def reccomend(inputFilePath,segment='women')
    	productArray = parse_data(inputFilePath)
    	productArray = clean_data(productArray)
    	calculate_similarities(CLUSTERER_INPUT_FILE_NAME,productArray)
    	queries = cluster(CLUSTERER_INPUT_FILE_NAME,productArray)
    	results = get_products(queries,productArray)
    	# preview everything
		data = ERB.new(template).result(binding)
		file = File.open(PREVIEW_FILE_NAME, "w")
		file.write(data)
		file.close
		
		system("open -a \"Google Chrome\" #{PREVIEW_FILE_NAME}")
    end
end

Cli.start

