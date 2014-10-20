/*
 * H.265 video codec.
 * Copyright (c) 2013-2014 struktur AG, Dirk Farin <farin@struktur.de>
 *
 * This file is part of libde265.
 *
 * libde265 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libde265 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libde265.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DE265_ENCPICBUF_H
#define DE265_ENCPICBUF_H

#include "libde265/image.h"
#include "libde265/sps.h"

#include <deque>
#include <vector>


class encoder_picture_buffer
{
 public:
  encoder_picture_buffer();
  ~encoder_picture_buffer();


  struct image_data
  {
    image_data();
    ~image_data();

    int frame_number;

    const de265_image* input;
    const de265_image* reconstruction;

    // SOP metadata

    std::vector<int> ref0;
    std::vector<int> ref1;
    std::vector<int> longterm;
    std::vector<int> keep;
    int sps_index;
    int temporal_layer;
    int skip_priority;
    bool is_intra;

    enum state {
      state_unprocessed,
      state_sop_metadata_available,
      state_encoding,
      state_keep_for_reference
    } state;

    bool mark_used;
  };


  // --- input pushed by the input process ---

  void reset();

  void insert_next_image_in_encoding_order(const de265_image*, int frame_number); // insert in encoding order
  void insert_end_of_stream();


  // --- SOP structure ---

  void set_image_intra();
  void set_image_references(int sps_index, // -1 -> custom
                            const std::vector<int>& l0, const std::vector<int>& l1,
                            const std::vector<int>& lt,
                            const std::vector<int>& keepMoreReferences);
  void set_temporal_layer(int temporal_layer);
  void set_skip_priority(int skip_priority);
  void sop_metadata_commit(int frame_number); // note: frame_number is only for consistency checking


  // --- infos pushed by encoder ---

  void mark_encoding_started(int frame_number);
  int  set_reconstruction_image(int frame_number, const de265_image*);
  void mark_encoding_finished(int frame_number);



  // --- data access ---

  bool have_more_frames_to_encode() const;
  const image_data* get_next_picture_to_encode(); // or return NULL if no picture is available
  const image_data* get_picture(int frame_number) const;

 private:
  bool mEndOfStream;
  std::deque<image_data*> mImages;

  void flush_images();
  image_data* get_picture(int frame_number);
};


#endif