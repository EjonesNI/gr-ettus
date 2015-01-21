/* -*- c++ -*- */
/* Copyright 2015 Ettus Research
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * gr-ettus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with gr-ettus; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "rfnoc_window_cci_impl.h"
#include <uhd/usrp/rfnoc/window_block_ctrl.hpp>
#include <boost/format.hpp>

namespace gr {
  namespace ettus {

    rfnoc_window_cci::sptr
    rfnoc_window_cci::make(
        const std::vector<int> &coeffs,
        const device3::sptr &dev,
        const int block_select,
        const int device_select
    ) {
      return gnuradio::get_initial_sptr(
          new rfnoc_window_cci_impl(coeffs, dev, block_select, device_select)
      );
    }

    rfnoc_window_cci_impl::rfnoc_window_cci_impl(
              const std::vector<int> &coeffs,
              const device3::sptr &dev,
              const int block_select,
              const int device_select
    ) : GR_RFNOC_BLOCK_SUPER_CTOR("rfnoc_window_cci"),
      d_window_size(coeffs.size())
    {
      ::uhd::stream_args_t stream_args("fc32", "sc16");
      stream_args.args["spp"] = str(boost::format("%s") % d_window_size);
      // Don't think we actually need this, but hey:
      stream_args.args["window_len"] = str(boost::format("%s") % d_window_size);
      GR_RFNOC_BLOCK_INIT(
          dev, rfnoc::rfnoc_common::make_block_id("Window", block_select, device_select),
          stream_args, stream_args
      );
      set_window(coeffs);
    }

    rfnoc_window_cci_impl::~rfnoc_window_cci_impl()
    {
      /* nop */
    }

    void rfnoc_window_cci_impl::set_window(const std::vector<int> &coeffs)
    {
      if (coeffs.size() != d_window_size) {
        throw std::runtime_error("Cannot change window size of running flow graph!");
      }
      get_block_ctrl_throw< ::uhd::rfnoc::window_block_ctrl >()->set_window(coeffs);
    }

  } /* namespace ettus */
} /* namespace gr */

