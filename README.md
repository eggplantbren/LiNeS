LiNeS (Linked Nested Sampling)
==============================

Nested Sampling is great. MCMC-based Nested Sampling is also great as it's
the most general way of doing things, but it can suck if your MCMC chains
get stuck.

LiNeS (Linked Nested Sampling)
is a way around this. First, it runs a classic MCMC-based NS run to
get an approximation of the logL-logX curve. It uses these to define a
discrete set of levels (like in Diffusive Nested Sampling) and then
runs Radford Neal's "Linked Importance Sampling" algorithm to
sample the sequence of levels.

The result will eventually be an embarrassingly parallel MCMC-based
NS algorithm that correctly compensates for imperfect MCMC sampling.

(c) 2016 Brendon J. Brewer

LICENCE: MIT. See the LICENSE file for details.

