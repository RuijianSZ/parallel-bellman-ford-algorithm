ITERATIONS=5

seq_file_output="seq_results.txt"
par_naive_file_output="par_naive_results.txt"
par_optim_file_output="par_optim_results.txt"
num_threads=4

# Run the Makefile to compile everything
make

# Run sequential algorithm, print info to file
# Clear the results file first, if it exists
rm -f ${seq_file_output}
# "echo" commands print some text to the screen, and 
# the ">>" redirects the output to a file and appends the text
# A plain "echo" command with no text will just print a new line
echo "===== Sequential =====" >> ${seq_file_output}
# Vary the number of vertices --> add more data points here
for num_vertices in 256 512 1024
do
	echo >> ${seq_file_output}
	# Run multiple iterations for each experiment, so we can take the average
	for it in $(seq 1 $ITERATIONS)
	do 
		echo "N_V = ${num_vertices} ITERATION${it}" >> ${seq_file_output}
		echo "Command: ./seq ${num_vertices}" >> ${seq_file_output}
		./seq ${num_vertices} >> ${seq_file_output}
	done
done

# Run naive parallel algorithm, print info to file
# Clear the results file first, if it exists
rm -f ${par_naive_file_output}
echo "===== Naive parallel =====" >> ${par_naive_file_output}
# Vary the number of vertices --> add more data points here
for num_vertices in 256 512 1024
do
	echo >> ${par_naive_file_output}
	# Run multiple iterations for each experiment, so we can take the average
	for it in $(seq 1 $ITERATIONS)
	do 
		echo "N_V = ${num_vertices} ITERATION${it}" >> ${par_naive_file_output}
		echo "Command: ./par-naive ${num_vertices} ${num_threads}" >> ${par_naive_file_output}
		./par-naive ${num_vertices} ${num_threads} >> ${par_naive_file_output}
	done
done

# Run optimized parallel algorithm, print info to file
# Clear the results file first, if it exists
rm -f ${par_optim_file_output}
echo "===== Optimized parallel =====" >> ${par_optim_file_output}
# Vary the number of vertices --> add more data points here
for num_vertices in 256 512 1024
do
	echo >> ${par_optim_file_output}
	# Run multiple iterations for each experiment, so we can take the average
	for it in $(seq 1 $ITERATIONS)
	do 
		echo "N_V = ${num_vertices} ITERATION${it}" >> ${par_optim_file_output}
		echo "Command: ./par-optim ${num_vertices} ${num_threads}" >> ${par_optim_file_output}
		./par-optim ${num_vertices} ${num_threads} >> ${par_optim_file_output}
	done
done


