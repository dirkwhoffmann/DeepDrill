# Smooth coloring

In the classical escape-time algorithm, the Mandelbrot iteration formula is applied until $|z_n|$ exceeds a certain limit, which is called the *escape radius* $R$. That is, the iteration is performed until the following condition holds:
\begin{align*}
|z_{n-1}| \leq R < |z_{n}|
\end{align*}
The value $n$ is called the *iteration count* for the computed pixel. In the classical coloring algorithm, $n$ is used as an index into a color table. In the early days of computer technology, this was not much of a problem because the number of colors available was usually limited. However, with the ever-increasing graphics capabilities of modern computers, people were looking for ways to replace the color bands with smooth color transitions. 

From a mathematical point of view, smoothing the color transition means to replace the integer value $n$ by a continuous floating point value $\nu$. The current standard solution is based on the *potential function*, which is defined as follows:
\begin{align*}
\phi(z) := \lim_{n \rightarrow \infty} \frac{\log |z_{n}|}{2^n}
\end{align*}
The intuition behind this formula is mathematically involved, so we'll just take its definition as granted. Just note that it is not at all clear that the limit for all values of $z$ exists at all. Nevertheless, it can be proved that $\phi(z)$ is a well-defined function. 

Now, suppose we have chosen a very large escape radius $R$. This means that by the time the escape condition kicks in, $n$ is already large enough to approximate the potential quite well. Hence, it no longer makes much difference whether we approximate $\phi(z)$ with $n$ or $n-1$. Symbolically, this can be expressed as follows:
\begin{align*}
\frac{\log |z_{n-1}|}{2^{n-1}} \approx \frac{\log |z_{n}|}{2^n}
\end{align*}
Note that $n$ has been chosen such that $|z_{n-1}|$ is less than $R$ and $|z_{n}|$ is greater than $R$. Now, the idea of smooth coloring is to replace $n$ with a fractional value $\nu$ that exactly hits $R$. In other words: The value $\nu$ we are looking for is supposed to satisfy the following condition:
\begin{align*}
\frac{\log |z_{n-1}|}{2^{n-1}} \approx \frac{\log R}{2^{\nu}} \approx \frac{\log |z_{n}|}{2^n}
\end{align*}
Note that from $|z_{n-1}| \leq R$ we can conclude that $n - 1 \leq \nu$. Similarly, we can conclude from $R < |z_{n}|$ that $\nu < n$. Thus $\nu$ is a fractional value satisfying the following property:
\begin{align*}
n - 1 \leq \nu < n
\end{align*}
We call $\nu$ the *fractional iteration count*. It coincides with $n$ if its fractional part is $0$, and interpolates in between otherwise.  

The formula for calculating $\nu$ can be easily derived from what has already been stated above:
\begin{align*}
\frac{\log R}{2^{\nu}} &\approx \frac{\log |z_{n}|}{2^n} \\
2^{n - \nu} &= \frac{\log |z_{n}|}{\log R} = \log_R |z_{n}| \\
n - \nu &= \log_2 \log_R |z_{n}| \\
\nu &= n - \log_2 \log_R |z_{n}|
\end{align*}
